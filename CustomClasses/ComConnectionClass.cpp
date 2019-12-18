//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdPort"
#pragma link "OoMisc"
//---------------------------------------------------------------------------
//�����������
TComConnection::TComConnection(TComponent* Owner, String _ComName, int _ComNumber, int ExpectationDelay,
								void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                                void (__closure *_ConnectionErrorTrigger)(TComConnection *, int), int BaudRate)
{
    ComPort = new TApdComPort(Owner);                   //������ ��������� COM-����� ��� ������ � COM-������
    ComPort->AutoOpen = false;  						//�������������� ����������� � ����� ���������
    ComPort->ComNumber = _ComNumber;                     //������������ ����� �����
    ComPort->Baud = BaudRate;                           //�������� ��������
    ComPort->OnTriggerAvail = ComPortOnTriggerAvail;    //����� ���������� ������� �� ������� ������
    ComPort->Name = "ComPort" + IntToStr(_ComNumber);
    ComNumber = _ComNumber;
    TimeoutTimer = new TTimer(Owner);                   //������ ������ ��� �������� ��������� ������
    													//(ComPortOnTriggerAvail ����������� �������� ��� ����� ���, ��� ����� ��� �������)
    TimeoutTimer->Enabled = false;                      //� ����� �������� ���
    TimeoutTimer->Interval = 50;                        //������������� �������� ������������
    TimeoutTimer->OnTimer = TimoutTimerOnTimer;         //����� ���������� ������� �������
    TimeoutTimer->Name = "TimeoutTimerForComPort" + IntToStr(ComNumber);

    ExpectationTimer = new TTimer(Owner);
    ExpectationTimer->Enabled = false;
    ExpectationTimer->Interval = ExpectationDelay;
    ExpectationTimer->OnTimer = ExpectationTimerOnTimer;
    ExpectationTimer->Name = "ExpectationTimerForComPort" + IntToStr(ComNumber);

    ComName = _ComName;
    DataReadyTrigger = _DataReadyTrigger;                             //����� ���������� ��������� ������
    ConnectionErrorTrigger = _ConnectionErrorTrigger;                 //����� ���������� ������ ����������

    try
    {
    	ComPort->Open = true;                         	//��������� COM-����
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);                    	//������ ��� �������� �����
        return;
    }

    ComPort->Open = false;
}
//---------------------------------------------------------------------------
//����������
TComConnection::~TComConnection()
{
	try
    {
		ComPort->Open = false;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionCloseError);
    }

	delete ComPort;
    delete TimeoutTimer;

    RecievedData.clear();
    RecievedData.~vector();
}
//---------------------------------------------------------------------------
void __fastcall TComConnection::ComPortOnTriggerAvail(TObject *CP, WORD Count)
{
	TimeoutTimer->Enabled = false;
    ExpectationTimer->Enabled = false;

    byte c;

	for (int i = 0; i < Count; i++) {
    	c = ComPort->GetChar();
        RecievedData.push_back(c);
	}

	TimeoutTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TComConnection::TimoutTimerOnTimer(TObject *Sender)
{
	TimeoutTimer->Enabled = false;
    ExpectationTimer->Enabled = false;
   	DataReadyTrigger(this, RecievedData);
    RecievedData.clear();
    try
    {
		ComPort->Open = false;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionCloseError);
    }
}
//---------------------------------------------------------------------------
void __fastcall TComConnection::ExpectationTimerOnTimer(TObject *Sender)
{
	TimeoutTimer->Enabled = false;
    ExpectationTimer->Enabled = false;
    try
    {
		ComPort->Open = false;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionCloseError);
    }
    ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);

}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{
	try
    {
    	ComPort->Open = true;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);
        return;
    }

	byte *Buf = new byte(Data.size());

    for(int i = 0; i < Data.size(); ++i)
    	Buf[i] = Data[i];

    try
    {
    	ComPort->PutBlock(Buf, Data.size()); 					//���������� ������
        ExpectationTimer->Enabled = true;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionDataPassError);//������ ��������
    }

    delete [] Buf;
}






//=============================================================================
//..................... ���������� ���������� ���������� ......................
//=============================================================================

#define BUFSIZE 255     //������� ������

unsigned char bufrd[BUFSIZE], bufwr[BUFSIZE]; //������� � ���������� ������

//---------------------------------------------------------------------------

HANDLE COMport;		//���������� �����

//��������� OVERLAPPED ���������� ��� ����������� ��������, ��� ���� ��� �������� ������ � ������ ����� �������� ������ ���������
//��� ��������� ���������� �������� ���������, ����� ��������� �� ����� �������� ���������
OVERLAPPED overlapped;		//����� ������������ ��� �������� ������ (��. ����� ReadThread)
OVERLAPPED overlappedwr;       	//����� ������������ ��� �������� ������ (��. ����� WriteThread)

//---------------------------------------------------------------------------

int handle;             	//���������� ��� ������ � ������ � ������� ���������� <io.h>

//---------------------------------------------------------------------------

bool fl=0;	//����, ����������� �� ���������� �������� ������ (1 - �������, 0 - �� �������)

unsigned long counter;	//������� �������� ������, ���������� ��� ������ �������� �����


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

void COMOpen(void);             //������� ����
void COMClose(void);            //������� ����


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

HANDLE reader;	//���������� ������ ������ �� �����
HANDLE writer;	//���������� ������ ������ � ����

DWORD WINAPI ReadThread(LPVOID);
DWORD WINAPI WriteThread(LPVOID);


//=============================================================================
//.............................. ���������� ������� ...........................
//=============================================================================

//-----------------------------------------------------------------------------
//............................... ����� ReadThead .............................
//-----------------------------------------------------------------------------

void ReadPrinting(void);

//---------------------------------------------------------------------------

//������� ������� ������, ��������� ���� ������ �� COM-�����
DWORD WINAPI ReadThread(LPVOID)
{
 COMSTAT comstat;		//��������� �������� ��������� �����, � ������ ��������� ������������ ��� ����������� ���������� �������� � ���� ������
 DWORD btr, temp, mask, signal;	//���������� temp ������������ � �������� ��������

 overlapped.hEvent = CreateEvent(NULL, true, true, NULL);	//������� ���������� ������-������� ��� ����������� ��������
 SetCommMask(COMport, EV_RXCHAR);                   	        //���������� ����� �� ������������ �� ������� ����� ����� � ����
 while(1)						//���� ����� �� ����� �������, ��������� ����
  {
   WaitCommEvent(COMport, &mask, &overlapped);               	//������� ������� ����� ����� (��� � ���� ������������� ��������)
   signal = WaitForSingleObject(overlapped.hEvent, INFINITE);	//������������� ����� �� ������� �����
   if(signal == WAIT_OBJECT_0)				        //���� ������� ������� ����� ���������
    {
     if(GetOverlappedResult(COMport, &overlapped, &temp, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
      if((mask & EV_RXCHAR)!=0)					//���� ��������� ������ ������� ������� �����
       {
        ClearCommError(COMport, &temp, &comstat);		//����� ��������� ��������� COMSTAT
        btr = comstat.cbInQue;                          	//� �������� �� �� ���������� �������� ������
        if(btr)                         			//���� ������������� ���� ����� ��� ������
        {
         ReadFile(COMport, bufrd, btr, &temp, &overlapped);     //��������� ����� �� ����� � ����� ���������
         counter+=btr;                                          //����������� ������� ������
         ReadPrinting();                      		//�������� ������� ��� ������ ������ �� ����� � � ����
        }
       }
    }
  }
}

//---------------------------------------------------------------------------

//������� �������� ����� �� ����� � � ���� (���� ��������)
void ReadPrinting()
{
 Form1->Memo1->Lines->Add((char*)bufrd);	 //������� �������� ������ � Memo
 Form1->StatusBar1->Panels->Items[2]->Text = "����� ������� " + IntToStr(counter) + " ����";	//������� ������� � ������ ���������

 if(Form1->CheckBox3->Checked == true)  //���� ������� ����� ������ � ����
  {
   write(handle, bufrd, strlen(bufrd)); //�������� � ���� ������ �� �������� ������
  }
 memset(bufrd, 0, BUFSIZE);	        //�������� ����� (����� ������ �� ������������� ���� �� �����)
}

//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//............................... ����� WriteThead ............................
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------

//������� ������� ������, ��������� �������� ������ �� ������ � COM-����
DWORD WINAPI WriteThread(LPVOID)
{
 DWORD temp, signal;	//temp - ����������-��������

 overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  //������� �������
 while(1)
  {WriteFile(COMport, bufwr, strlen(bufwr), &temp, &overlappedwr);  //�������� ����� � ���� (������������� ��������!)
   signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE);	  //������������� �����, ���� �� ���������� ������������� �������� WriteFile

   if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//���� �������� ����������� �������
     {
      Form1->StatusBar1->Panels->Items[0]->Text  = "�������� ������ �������";    //������� ��������� �� ���� � ������ ���������
     }
   else {Form1->StatusBar1->Panels->Items[0]->Text  = "������ ��������";} 	//����� ������� � ������ ��������� ��������� �� ������

   SuspendThread(writer);

  }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//������ "��������"
void __fastcall TForm1::Button1Click(TObject *Sender)
{
 memset(bufwr,0,BUFSIZE);			//�������� ����������� ���������� �����, ����� ������ �� ������������� ���� �� �����
 PurgeComm(COMport, PURGE_TXCLEAR);             //�������� ���������� ����� �����
 strcpy(bufwr,Form1->Edit1->Text.c_str());      //������� � ����������� ���������� ����� ������ �� Edit1

 ResumeThread(writer);               //������������ ����� ������ ������ � ����

}
//---------------------------------------------------------------------------

//=============================================================================
//........................... ���������� ������� ..............................
//=============================================================================

//---------------------------------------------------------------------------

//������� �������� � ������������� �����
void COMOpen()
{
 String portname;     	 //��� ����� (��������, "COM1", "COM2" � �.�.)
 DCB dcb;                //��������� ��� ����� ������������� ����� DCB
 COMMTIMEOUTS timeouts;  //��������� ��� ��������� ���������

 portname = Form1->ComboBox1->Text;	//�������� ��� ���������� �����

 //������� ����, ��� ����������� �������� ����������� ����� ������� ���� FILE_FLAG_OVERLAPPED
 COMport = CreateFile(portname.c_str(),GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
 //�����:
 // - portname.c_str() - ��� ����� � �������� ����� �����, c_str() ����������� ������ ���� String � ������ � ���� ������� ���� char, ����� ������� �� ������
 // - GENERIC_READ | GENERIC_WRITE - ������ � ����� �� ������/�������
 // - 0 - ���� �� ����� ���� ������������� (shared)
 // - NULL - ���������� ����� �� �����������, ������������ ���������� ������������ �� ���������
 // - OPEN_EXISTING - ���� ������ ����������� ��� ��� ������������ ����
 // - FILE_FLAG_OVERLAPPED - ���� ���� ��������� �� ������������� ����������� ��������
 // - NULL - ��������� �� ���� ������� �� ������������ ��� ������ � �������

 if(COMport == INVALID_HANDLE_VALUE)            //���� ������ �������� �����
  {
   Form1->SpeedButton1->Down = false;           //������ ������
   Form1->StatusBar1->Panels->Items[0]->Text = "�� ������� ������� ����";       //������� ��������� � ������ ���������
   return;
  }

 //������������� �����

 dcb.DCBlength = sizeof(DCB); 	//� ������ ���� ��������� DCB ���������� ������� � �����, ��� ����� �������������� ��������� ��������� ����� ��� �������� ������������ ���������

 //������� ��������� DCB �� �����
 if(!GetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "�� ������� ������� DCB";
   return;
  }

 //������������� ��������� DCB
 dcb.BaudRate = StrToInt(Form1->ComboBox2->Text);       //����� �������� �������� 115200 ���
 dcb.fBinary = TRUE;                                    //�������� �������� ����� ������
 dcb.fOutxCtsFlow = FALSE;                              //��������� ����� �������� �� �������� CTS
 dcb.fOutxDsrFlow = FALSE;                              //��������� ����� �������� �� �������� DSR
 dcb.fDtrControl = DTR_CONTROL_DISABLE;                 //��������� ������������� ����� DTR
 dcb.fDsrSensitivity = FALSE;                           //��������� ��������������� �������� � ��������� ����� DSR
 dcb.fNull = FALSE;                                     //��������� ���� ������� ������
 dcb.fRtsControl = RTS_CONTROL_DISABLE;                 //��������� ������������� ����� RTS
 dcb.fAbortOnError = FALSE;                             //��������� ��������� ���� �������� ������/������ ��� ������
 dcb.ByteSize = 8;                                      //����� 8 ��� � �����
 dcb.Parity = 0;                                        //��������� �������� ��������
 dcb.StopBits = 0;                                      //����� ���� ����-���

 //��������� ��������� DCB � ����
 if(!SetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "�� ������� ���������� DCB";
   return;
  }

 //���������� ��������
 timeouts.ReadIntervalTimeout = 0;	 	//������� ����� ����� ���������
 timeouts.ReadTotalTimeoutMultiplier = 0;	//����� ������� �������� ������
 timeouts.ReadTotalTimeoutConstant = 0;         //��������� ��� ������ �������� �������� ������
 timeouts.WriteTotalTimeoutMultiplier = 0;      //����� ������� �������� ������
 timeouts.WriteTotalTimeoutConstant = 0;        //��������� ��� ������ �������� �������� ������

 //�������� ��������� ��������� � ����
 if(!SetCommTimeouts(COMport, &timeouts))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "�� ������� ���������� ����-����";
   return;
  }

 //���������� ������� �������� ����� � ��������
 SetupComm(COMport,2000,2000);

 //������� ��� ������� ������������ ���� ��� ������ ����������� ������
 handle = open("test.txt", O_CREAT | O_APPEND | O_BINARY | O_WRONLY, S_IREAD | S_IWRITE);

 if(handle==-1)		//���� ��������� ������ �������� �����
  {
   Form1->StatusBar1->Panels->Items[1]->Text = "������ �������� �����";	//������� ��������� �� ���� � ��������� ������
   Form1->Label6->Hide();                                               //�������� ������� � ������ �����
   Form1->CheckBox3->Checked = false;                                   //�������� � ��������� �������
   Form1->CheckBox3->Enabled = false;
  }
 else { Form1->StatusBar1->Panels->Items[0]->Text = "���� ������ �������"; } //����� ������� � ������ ��������� ��������� �� �������� �������� �����

 PurgeComm(COMport, PURGE_RXCLEAR);	//�������� ����������� ����� �����

 reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//������ ����� ������, ������� ����� ����� ����������� (������������� �������� = 0)
 writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//������ ����� ������ � ������������� ��������� (������������� �������� = CREATE_SUSPENDED)

}

//---------------------------------------------------------------------------

//������� �������� �����
void COMClose()
{
  //����������: ��� ��� ��� ���������� �������, ��������� � ������� ������� WinAPI, �������� TerminateThread
  //	      ����� ����� ���� ������� �����, � ����� ����� ������ ����������, �� ����������� ����������
  //	      ����������� �������-�������, ������������ � ��������� ���� OVERLAPPED, ��������� � �������,
  //	      ������� �� ������ ���� ������, � ��������, ����� ������ ������� TerminateThread.
  //	      ����� ���� ����� ���������� � ��� ���������� ������.

 if(writer)		//���� ����� ������ ��������, ��������� ���; �������� if(writer) �����������, ����� ��������� ������
  {TerminateThread(writer,0);
   CloseHandle(overlappedwr.hEvent);	//����� ������� ������-�������
   CloseHandle(writer);
  }
 if(reader)		   //���� ����� ������ ��������, ��������� ���; �������� if(reader) �����������, ����� ��������� ������
  {TerminateThread(reader,0);
   CloseHandle(overlapped.hEvent);	//����� ������� ������-�������
   CloseHandle(reader);
  }

 CloseHandle(COMport);                  //������� ����
 COMport=0;				//�������� ���������� ��� ����������� �����
 close(handle);				//������� ����, � ������� ������ ������ ����������� ������
 handle=0;				//�������� ���������� ��� ����������� �����

}

//---------------------------------------------------------------------------

