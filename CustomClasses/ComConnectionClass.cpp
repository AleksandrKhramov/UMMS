//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
OVERLAPPED overlapped;		//����� ������������ ��� �������� ������ (��. ����� ReadThread)
OVERLAPPED overlappedwr;       	//����� ������������ ��� �������� ������ (��. ����� WriteThread)
//�����������
TComConnection::TComConnection(String _ComName, int _ComNumber, int _ExpectationDelay,
								void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                                void (__closure *_ConnectionErrorTrigger)(TComConnection *, int), int BaudRate)
{
    ExpectationDelay = _ExpectationDelay;

    DataReadyTrigger = _DataReadyTrigger;                             //����� ���������� ��������� ������
    ConnectionErrorTrigger = _ConnectionErrorTrigger;                 //����� ���������� ������ ����������

    try
    {
    	COMOpen(_ComNumber, BaudRate);                        									//��������� COM-����
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);                    	//������ ��� �������� �����
        return;
    }
   	COMClose();
}
//---------------------------------------------------------------------------
//����������
TComConnection::~TComConnection()
{

}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{
	try
    {
    	PurgeComm(COMport, PURGE_TXCLEAR);             //�������� ���������� ����� �����

    	DWORD temp, signal;	//temp - ����������-��������
        overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  		//������� �������

        byte *Buffer = new byte(Data.size());
        for (int i = 0; i < Data.size(); ++i)
        	Buffer[i] = Data[i];

        WriteFile(COMport, Buffer, Data.size(), &temp, &overlappedwr);  	//�������� ����� � ���� (������������� ��������!)
        signal = WaitForSingleObject(overlappedwr.hEvent, 100);	  			//������������� �����, ���� �� ���������� ������������� �������� WriteFile

        if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//���� �������� ����������� �������
        {
        	WaitAnswer();
        }
        else
        {
        	ConnectionErrorTrigger(this, ComConnectionDataPassError);
        }
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);
        return;
    }
}
//---------------------------------------------------------------------------
void TComConnection::WaitAnswer()
{
	COMSTAT comstat;		//��������� �������� ��������� �����, � ������ ��������� ������������ ��� ����������� ���������� �������� � ���� ������
    DWORD RecievedByteCount, temp, mask, signal;	//���������� temp ������������ � �������� ��������
    std::vector<byte> RecievedData;

    overlapped.hEvent = CreateEvent(NULL, true, true, NULL);	//������� ���������� ������-������� ��� ����������� ��������
    SetCommMask(COMport, EV_RXCHAR);                   	        //���������� ����� �� ������������ �� ������� ����� ����� � ����

    WaitCommEvent(TComConnection::COMport, &mask, &overlapped);               	//������� ������� ����� ����� (��� � ���� ������������� ��������)
    signal = WaitForSingleObject(overlapped.hEvent, 200);	//������������� ����� �� ������� �����

    if(signal == WAIT_OBJECT_0)				        //���� ������� ������� ����� ���������
    {
        if(GetOverlappedResult(COMport, &overlapped, &temp, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
            if((mask & EV_RXCHAR)!=0)					//���� ��������� ������ ������� ������� �����
            {
                ClearCommError(COMport, &temp, &comstat);		//����� ��������� ��������� COMSTAT
                RecievedByteCount = comstat.cbInQue;                          	//� �������� �� �� ���������� �������� ������
                byte *Buffer = new byte(RecievedByteCount);
                if(RecievedByteCount)                         					//���� ������������� ���� ����� ��� ������
                {
                	ReadFile(COMport, Buffer, RecievedByteCount, &temp, &overlapped);     //��������� ����� �� ����� � ����� ���������

                    for(byte i = 0; i < RecievedByteCount; ++i)
                    {
                    	RecievedData.push_back(Buffer[i]);
                    }
                    DataReadyTrigger(this, RecievedData);
                }
                else
                {
                	ConnectionErrorTrigger(this, ComConnectionReceivingDataError);
                }
                delete [] Buffer;
            }
    }

}
//---------------------------------------------------------------------------
//������� �������� � ������������� �����
void TComConnection::COMOpen(int _ComNumber, int _BaudRate)
{
	String TempPortName;     	 //��� ����� (��������, "COM1", "COM2" � �.�.)
    DCB dcb;                	//��������� ��� ����� ������������� ����� DCB
    COMMTIMEOUTS Timeouts;  	//��������� ��� ��������� ���������

 	TempPortName = "COM" + IntToStr(_ComNumber);	//�������� ��� ���������� �����

    COMport = CreateFile((const char*)TempPortName.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if(COMport == INVALID_HANDLE_VALUE)            //���� ������ �������� �����
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);
    	return;
    }

 	//������������� �����

 	dcb.DCBlength = sizeof(DCB); 	//� ������ ���� ��������� DCB ���������� ������� � �����, ��� ����� �������������� ��������� ��������� ����� ��� �������� ������������ ���������

    //������� ��������� DCB �� �����
    if(!GetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  	{
    	COMClose();
       	ConnectionErrorTrigger(this, ComConnectionOpenError);
       	return;
  	}

 	//������������� ��������� DCB
 	dcb.BaudRate = _BaudRate;       					   //����� �������� ��������
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
   		ConnectionErrorTrigger(this, ComConnectionOpenError);
   		return;
  	}

 	//���������� ��������
 	Timeouts.ReadIntervalTimeout = 0;	 			//������� ����� ����� ���������
 	Timeouts.ReadTotalTimeoutMultiplier = 0;	//����� ������� �������� ������
 	Timeouts.ReadTotalTimeoutConstant = 0;         //��������� ��� ������ �������� �������� ������
 	Timeouts.WriteTotalTimeoutMultiplier = 0;      //����� ������� �������� ������
 	Timeouts.WriteTotalTimeoutConstant = 0;        //��������� ��� ������ �������� �������� ������

 	//�������� ��������� ��������� � ����
 	if(!SetCommTimeouts(COMport, &Timeouts))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  	{
   		COMClose();
   		ConnectionErrorTrigger(this, ComConnectionOpenError);
   		return;
  	}

 	//���������� ������� �������� ����� � ��������
 	SetupComm(COMport, 2000, 2000);

 	PurgeComm(COMport, PURGE_RXCLEAR);	//�������� ����������� ����� �����

 	//reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//������ ����� ������, ������� ����� ����� ����������� (������������� �������� = 0)
 	//writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//������ ����� ������ � ������������� ��������� (������������� �������� = CREATE_SUSPENDED)
}
//---------------------------------------------------------------------------
void TComConnection::COMClose()
{
	/*if(writer)		//���� ����� ������ ��������, ��������� ���; �������� if(writer) �����������, ����� ��������� ������
  	{
        TerminateThread(writer,0);
        CloseHandle(overlappedwr.hEvent);	//����� ������� ������-�������
        CloseHandle(writer);
  	}

 	if(reader)		   //���� ����� ������ ��������, ��������� ���; �������� if(reader) �����������, ����� ��������� ������
  	{
        TerminateThread(reader,0);
        CloseHandle(overlapped.hEvent);	//����� ������� ������-�������
        CloseHandle(reader);
  	}    */

    CloseHandle(COMport);                  //������� ����
    COMport = NULL;				//�������� ���������� ��� ����������� �����

}
//---------------------------------------------------------------------------

