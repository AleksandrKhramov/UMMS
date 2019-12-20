//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

//Конструктор
TComConnection::TComConnection(String _ComName, int _ComNumber, int _ExpectationDelay,
								void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                                void (__closure *_ConnectionErrorTrigger)(TComConnection *, int), int BaudRate)
{
    ExpectationDelay = _ExpectationDelay;

    DataReadyTrigger = _DataReadyTrigger;                             //Задаём обработчик принятого пакета
    ConnectionErrorTrigger = _ConnectionErrorTrigger;                 //Задаём обработчик ошибок соединения

    try
    {
    	COMOpen(_ComNumber, BaudRate);                        									//Открываем COM-порт
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);                    	//Ошибка при открытии порта
        return;
    }
}
//---------------------------------------------------------------------------
//Деструктор
TComConnection::~TComConnection()
{

}
//---------------------------------------------------------------------------
void TComConnection::Write(std::vector<byte> Data)
{
 	if(COMport == INVALID_HANDLE_VALUE) {

        ConnectionErrorTrigger(this, ComConnectionDataPassError);
 		return;
 	}

 	DWORD feedback;
    byte *Buffer = new byte[Data.size()];
    for (int i = 0; i < Data.size(); ++i)
    	Buffer[i] = Data[i];

 	if(!WriteFile(COMport, Buffer, Data.size(), &feedback, NULL) || feedback != Data.size()) {
 		COMClose();
 		ConnectionErrorTrigger(this, feedback);
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
void TComConnection::SendData(std::vector<byte> Data)
{
	try
    {
    	COMOpen(ComNumber);

    	PurgeComm(COMport, PURGE_TXCLEAR);             //очистить передающий буфер порта

    	DWORD temp, signal;	//temp - переменная-заглушка
        overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  		//создать событие

        byte *Buffer = new byte[Data.size()];
        for (int i = 0; i < Data.size(); ++i)
        	Buffer[i] = Data[i];

        WriteFile(COMport, Buffer, Data.size(), &temp, 0);  	//записать байты в порт (перекрываемая операция!)

        ConnectionErrorTrigger(this, temp);
        return;

        signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE);	  			//приостановить поток, пока не завершится перекрываемая операция WriteFile

        if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//если операция завершилась успешно
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
	std::vector<byte> Data;

	int TIMEOUT = 1000;
 	if(COMport == INVALID_HANDLE_VALUE)
    {
 		ConnectionErrorTrigger(this, ComConnectionReceivingDataError);
        return;

    }

    Sleep(300);

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
        ConnectionErrorTrigger(this, feedback);
 	}


 	if(len) {
 		ConnectionErrorTrigger(this, ComConnectionReceivingDataError);
 	}

    for (int i = 0; i < RecievedByteCount; i++)
    {
    	Data.push_back(buf[i]);
    }
    DataReadyTrigger(this,Data);

}
//---------------------------------------------------------------------------
//функция открытия и инициализации порта
void TComConnection::COMOpen(int _ComNumber, int _BaudRate)
{

	String TempPortName;     	 //имя порта (например, "COM1", "COM2" и т.д.)
    DCB dcb;                	//структура для общей инициализации порта DCB
    COMMTIMEOUTS Timeouts;  	//структура для установки таймаутов

 	TempPortName = "COM" + IntToStr(_ComNumber);	//получить имя выбранного порта

    COMport = CreateFile(AnsiString(TempPortName).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(COMport == INVALID_HANDLE_VALUE)            //если ошибка открытия порта
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenInvalidHandleError);
    	return;
    }
 	//инициализация порта

 	dcb.DCBlength = sizeof(DCB); 	//в первое поле структуры DCB необходимо занести её длину, она будет использоваться функциями настройки порта для контроля корректности структуры

    //считать структуру DCB из порта
    if(!GetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  	{
    	COMClose();
       	ConnectionErrorTrigger(this, ComConnectionOpenGetStateError);
       	return;
  	}

 	//инициализация структуры DCB
 	dcb.BaudRate = 9600;       					   //задаём скорость передачи
    dcb.fBinary = TRUE;                                    //включаем двоичный режим обмена
    dcb.fOutxCtsFlow = FALSE;                              //выключаем режим слежения за сигналом CTS
    dcb.fOutxDsrFlow = FALSE;                              //выключаем режим слежения за сигналом DSR
    dcb.fDtrControl = DTR_CONTROL_DISABLE;                 //отключаем использование линии DTR
    dcb.fDsrSensitivity = FALSE;                           //отключаем восприимчивость драйвера к состоянию линии DSR
    dcb.fNull = FALSE;                                     //разрешить приём нулевых байтов
    dcb.fRtsControl = RTS_CONTROL_DISABLE;                 //отключаем использование линии RTS
    dcb.fAbortOnError = FALSE;                             //отключаем остановку всех операций чтения/записи при ошибке
    dcb.ByteSize = 8;                                      //задаём 8 бит в байте
    dcb.Parity = 0;                                        //отключаем проверку чётности
    dcb.StopBits = 0;                                      //задаём один стоп-бит

 	//загрузить структуру DCB в порт
 	if(!SetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  	{
    	COMClose();
   		ConnectionErrorTrigger(this, ComConnectionOpenSetStateError);
   		return;
  	}

 	//установить таймауты
 	Timeouts.ReadIntervalTimeout = 0;	 			//таймаут между двумя символами
 	Timeouts.ReadTotalTimeoutMultiplier = 100;		//общий таймаут операции чтения
 	Timeouts.ReadTotalTimeoutConstant = 0;         //константа для общего таймаута операции чтения
 	Timeouts.WriteTotalTimeoutMultiplier = 0;      //общий таймаут операции записи
 	Timeouts.WriteTotalTimeoutConstant = 0;        //константа для общего таймаута операции записи

 	//записать структуру таймаутов в порт
 	if(!SetCommTimeouts(COMport, &Timeouts))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  	{
   		COMClose();
   		ConnectionErrorTrigger(this, ComConnectionOpenSetTimeoutsError);
   		return;
  	}

 	//установить размеры очередей приёма и передачи
 	SetupComm(COMport, 2000, 2000);

 	PurgeComm(COMport, PURGE_RXCLEAR);	//очистить принимающий буфер порта

 	//reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//создаём поток чтения, который сразу начнёт выполняться (предпоследний параметр = 0)
 	//writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//создаём поток записи в остановленном состоянии (предпоследний параметр = CREATE_SUSPENDED)
}
//---------------------------------------------------------------------------
void TComConnection::COMClose()
{
    CloseHandle(COMport);                  //закрыть порт
    COMport = NULL;				//обнулить переменную для дескриптора порта
}
//---------------------------------------------------------------------------

