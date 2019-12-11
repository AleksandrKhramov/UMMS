//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdPort"
#pragma link "OoMisc"
//---------------------------------------------------------------------------
//Конструктор
TComConnection::TComConnection(TComponent* Owner, String _ComName, int _ComNumber,
								void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                                void (__closure *_ConnectionErrorTrigger)(TComConnection *, int), int BaudRate)
{
	ComPort = new TApdComPort(Owner);                   //Создаём компонент COM-порта для работы с COM-портом
	ComPort->AutoOpen = false;  						//Автоматическое подключение к порту отключено
    ComPort->ComNumber = _ComNumber;                     //Устанвливаем номер порта
    ComPort->Baud = BaudRate;                           //Скорость передачи
    ComPort->OnTriggerAvail = ComPortOnTriggerAvail;    //Задаём обработчик события по приходу данных
    ComPort->Name = "ComPort" + IntToStr(_ComNumber);
    ComNumber = _ComNumber;
    TimeoutTimer = new TTimer(Owner);                   //Создаём таймер для задержки обработки данных
    													//(ComPortOnTriggerAvail срабатывает нескольо раз перед тем, как придёт вся посылка)
    TimeoutTimer->Enabled = false;                      //И сразу отключем его
    TimeoutTimer->Interval = 50;                        //устанавливаем интервал срабатывания
    TimeoutTimer->OnTimer = TimoutTimerOnTimer;         //Задаём обработчик события таймера
    TimeoutTimer->Name = "TimeoutTimerForComPort" + IntToStr(ComNumber);

    ComName = _ComName;
    DataReadyTrigger = _DataReadyTrigger;                             //Задаём обработчик принятого пакета
    ConnectionErrorTrigger = _ConnectionErrorTrigger;                 //Задаём обработчик ошибок соединения

    try
    {
    	ComPort->Open = true;                         	//Открываем COM-порт
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);                    	//Ошибка при открытии порта
    }
}
//---------------------------------------------------------------------------
//Деструктор
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
   	DataReadyTrigger(this, RecievedData);
    RecievedData.clear();
}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{
	byte *Buf = new byte(Data.size());

    for(int i = 0; i < Data.size(); ++i)
    	Buf[i] = Data[i];

    try
    {
    	ComPort->PutBlock(Buf, Data.size()); 					//Отправляем данные
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionDataPassError);             		//Ошибка передачи
    }

    delete [] Buf;
}
