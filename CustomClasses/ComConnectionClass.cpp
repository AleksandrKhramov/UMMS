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
    ComName = _ComName;

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

 	DWORD NumberOfBytesWritten;

 	if(!WriteFile(COMport, &Data[0], Data.size(), &NumberOfBytesWritten, NULL) || NumberOfBytesWritten != Data.size()) {
 		COMClose();
 		ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);
 		return;
 	}
    WaitAnswer();
    COMClose();
}
//---------------------------------------------------------------------------
void TComConnection::WaitAnswer()
{
 	if(COMport == INVALID_HANDLE_VALUE)
    {
 		ConnectionErrorTrigger(this, ComConnectionReceivingDataHandleError);
        return;
    }

    Sleep(ExpectationDelay);

    std::vector<byte> Data;
 	DWORD NumberOfBytesRead = 0;

    DWORD RecievedByteCount, Errors;
    COMSTAT DeviceStatusInfo;

    ClearCommError(COMport, &Errors, &DeviceStatusInfo);
    RecievedByteCount = DeviceStatusInfo.cbInQue;

    Data.resize(RecievedByteCount);

    if(!ReadFile(COMport, &Data[0], RecievedByteCount, &NumberOfBytesRead, NULL))
    	ConnectionErrorTrigger(this, ComConnectionReceivingDataError);

    if(RecievedByteCount != NumberOfBytesRead)
    	ConnectionErrorTrigger(this, ComConnectionReceivingDataCountError);

    if(RecievedByteCount)
    	DataReadyTrigger(this, Data);
    else
     	ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);
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
}
//---------------------------------------------------------------------------
void TComConnection::COMClose()
{
    CloseHandle(COMport);                  //������� ����
    COMport = NULL;				//�������� ���������� ��� ����������� �����
}
//---------------------------------------------------------------------------
void TComConnection::ConnectionTest()
{
    COMOpen(ComNumber);
	COMClose();
}
