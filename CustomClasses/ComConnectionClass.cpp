//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

//�����������
TComConnection::TComConnection(String _ComName, int _ComNumber, int _ExpectationDelay,
								void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                                void (__closure *_ConnectionErrorTrigger)(TComConnection *, int), int BaudRate)
{
    ExpectationDelay = _ExpectationDelay;

    DataReadyTrigger = _DataReadyTrigger;                             //����� ���������� ��������� ������
    ConnectionErrorTrigger = _ConnectionErrorTrigger;                 //����� ���������� ������ ����������
    ComNumber = _ComNumber;
    try
    {
    	COMOpen(_ComNumber, BaudRate);                        									//��������� COM-����
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);                    	//������ ��� �������� �����
        return;
    }
}
//---------------------------------------------------------------------------
//����������
TComConnection::~TComConnection()
{
	COMClose();
}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{

 	if(COMport == INVALID_HANDLE_VALUE)
    {
    	COMOpen(ComNumber);
        if(COMport == INVALID_HANDLE_VALUE)
        {
            ConnectionErrorTrigger(this, ComConnectionDataPassError);
            return;
        }
 	}

 	DWORD feedback;
    byte *Buffer = new byte[Data.size()];
    for (int i = 0; i < Data.size(); ++i)
    	Buffer[i] = Data[i];

 	if(!WriteFile(COMport, Buffer, Data.size(), &feedback, NULL) || feedback != Data.size()) {
 		COMClose();
 		ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);
        delete Buffer;
 		return;
 	}
    WaitAnswer();
    delete Buffer;
    COMClose();
 	// In some cases it's worth uncommenting
 	//FlushFileBuffers(m_Handle);

}
//---------------------------------------------------------------------------
void TComConnection::WaitAnswer()
{
	std::vector<byte> Data;

	int TIMEOUT = 1000;
 	if(COMport == INVALID_HANDLE_VALUE)
    {
 		ConnectionErrorTrigger(this, ComConnectionReceivingDataHandleError);
        return;

    }

    Sleep(ExpectationDelay);

 	DWORD begin = GetTickCount();
 	DWORD feedback = 0;

    DWORD RecievedByteCount, temp;
    COMSTAT comstat;
    ClearCommError(COMport, &temp, &comstat);
    RecievedByteCount = comstat.cbInQue;

 	unsigned char* buf = new byte[RecievedByteCount];

 	DWORD len = RecievedByteCount;

 	int attempts = 3;

 	while(len && (attempts || (GetTickCount()-begin) < (DWORD)TIMEOUT/3))
    {

 		if(attempts) attempts--;

 		if(!ReadFile(COMport, buf, RecievedByteCount, &feedback, NULL))
        {
 			ConnectionErrorTrigger(this, ComConnectionReceivingDataError);
 		}

 		assert(feedback <= len);
 		len -= feedback;
 	}

    for (int i = 0; i < RecievedByteCount; i++)
    {
    	Data.push_back(buf[i]);
    }

    if(RecievedByteCount)
    	DataReadyTrigger(this, Data);
    else
    {
     	ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);
    }

}
//---------------------------------------------------------------------------
//������� �������� � ������������� �����
void TComConnection::COMOpen(int _ComNumber, int _BaudRate)
{

	String TempPortName;     	 //��� ����� (��������, "COM1", "COM2" � �.�.)
    DCB dcb;                	//��������� ��� ����� ������������� ����� DCB
    COMMTIMEOUTS Timeouts;  	//��������� ��� ��������� ���������

 	TempPortName = "\\\\.\\COM" + IntToStr(_ComNumber);	//�������� ��� ���������� �����

    COMport = CreateFile(AnsiString(TempPortName).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(COMport == INVALID_HANDLE_VALUE)            //���� ������ �������� �����
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenInvalidHandleError);
    	return;
    }
 	//������������� �����

 	dcb.DCBlength = sizeof(DCB); 	//� ������ ���� ��������� DCB ���������� ������� � �����, ��� ����� �������������� ��������� ��������� ����� ��� �������� ������������ ���������

    //������� ��������� DCB �� �����
    if(!GetCommState(COMport, &dcb))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  	{
    	COMClose();
       	ConnectionErrorTrigger(this, ComConnectionOpenGetStateError);
       	return;
  	}

 	//������������� ��������� DCB
 	dcb.BaudRate = 9600;       					   //����� �������� ��������
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
   		ConnectionErrorTrigger(this, ComConnectionOpenSetStateError);
   		return;
  	}

 	//���������� ��������
 	Timeouts.ReadIntervalTimeout = 0;	 			//������� ����� ����� ���������
 	Timeouts.ReadTotalTimeoutMultiplier = 100;		//����� ������� �������� ������
 	Timeouts.ReadTotalTimeoutConstant = 0;         //��������� ��� ������ �������� �������� ������
 	Timeouts.WriteTotalTimeoutMultiplier = 0;      //����� ������� �������� ������
 	Timeouts.WriteTotalTimeoutConstant = 0;        //��������� ��� ������ �������� �������� ������

 	//�������� ��������� ��������� � ����
 	if(!SetCommTimeouts(COMport, &Timeouts))	//���� �� ������� - ������� ���� � ������� ��������� �� ������ � ������ ���������
  	{
   		COMClose();
   		ConnectionErrorTrigger(this, ComConnectionOpenSetTimeoutsError);
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
    CloseHandle(COMport);                  //������� ����
    COMport = NULL;				//�������� ���������� ��� ����������� �����
}
//---------------------------------------------------------------------------

