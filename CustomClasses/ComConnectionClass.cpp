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
    }
}
//---------------------------------------------------------------------------
//����������
TComConnection::~TComConnection()
{
	ComPort->Open = false;
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
}
//---------------------------------------------------------------------------
void __fastcall TComConnection::ExpectationTimerOnTimer(TObject *Sender)
{
	TimeoutTimer->Enabled = false;
    ExpectationTimer->Enabled = false;
    ConnectionErrorTrigger(this, ComConnectionDataPassExpectationError);
}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{
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
