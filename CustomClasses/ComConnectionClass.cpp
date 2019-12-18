//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
OVERLAPPED overlapped;		//будем использовать для операций чтения (см. поток ReadThread)
OVERLAPPED overlappedwr;       	//будем использовать для операций записи (см. поток WriteThread)
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
   	COMClose();
}
//---------------------------------------------------------------------------
//Деструктор
TComConnection::~TComConnection()
{

}
//---------------------------------------------------------------------------
void TComConnection::SendData(std::vector<byte> Data)
{
	try
    {
    	TransmittingData = Data;
        PurgeComm(COMport, PURGE_TXCLEAR);             //очистить передающий буфер порта

        ResumeThread(writer);               //активировать поток записи данных в порт
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);
        return;
    }
}
//---------------------------------------------------------------------------
DWORD WINAPI TComConnection::ReadThread(LPVOID)
{
	COMSTAT comstat;		//структура текущего состояния порта, в данной программе используется для определения количества принятых в порт байтов
    DWORD RecievedByteCount, temp, mask, signal;	//переменная temp используется в качестве заглушки
    std::vector<byte> RecievedData;

    overlapped.hEvent = CreateEvent(NULL, true, true, NULL);	//создать сигнальный объект-событие для асинхронных операций
    SetCommMask(COMport, EV_RXCHAR);                   	        //установить маску на срабатывание по событию приёма байта в порт
 	while(1)						//пока поток не будет прерван, выполняем цикл
  	{
    	WaitCommEvent(COMport, &mask, &overlapped);               	//ожидать события приёма байта (это и есть перекрываемая операция)
       	signal = WaitForSingleObject(overlapped.hEvent, INFINITE);	//приостановить поток до прихода байта
   		if(signal == WAIT_OBJECT_0)				        //если событие прихода байта произошло
    	{
        	if(GetOverlappedResult(COMport, &overlapped, &temp, true)) //проверяем, успешно ли завершилась перекрываемая операция WaitCommEvent
      			if((mask & EV_RXCHAR)!=0)					//если произошло именно событие прихода байта
                {
                    ClearCommError(COMport, &temp, &comstat);		//нужно заполнить структуру COMSTAT
                    RecievedByteCount = comstat.cbInQue;                          	//и получить из неё количество принятых байтов
                    byte *Buffer = new byte(RecievedByteCount);
                    if(RecievedByteCount)                         					//если действительно есть байты для чтения
                    {
                    	ReadFile(COMport, Buffer, RecievedByteCount, &temp, &overlapped);     //прочитать байты из порта в буфер программы

                        for (byte i = 0; i < RecievedByteCount; ++i)
                        {
                        	RecievedData.push_back(Buffer[i]);
                        }
                     	DataReadyTrigger(this, RecievedData);
    					RecievedData.clear();
        			}
                    else
                    {
                    	ConnectionErrorTrigger(this, ComConnectionReceivingDataError);
                    }
                    delete [] Buffer;
       			}
    	}
  	}
}
//---------------------------------------------------------------------------
DWORD WINAPI WriteThread(LPVOID)
{
	DWORD temp, signal;	//temp - переменная-заглушка

 	overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  		//создать событие
 	while(1)
  	{
    	byte *Buffer = new byte(TComConnection::TransmittingData.size());

    	WriteFile(TComConnection::COMport, bufwr, strlen(bufwr), &temp, &overlappedwr);  	//записать байты в порт (перекрываемая операция!)
   		signal = WaitForSingleObject(overlappedwr.hEvent, 100);	  			//приостановить поток, пока не завершится перекрываемая операция WriteFile

   		if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//если операция завершилась успешно
     	{

     	}
   		else
        {
        	ConnectionErrorTrigger(this, ComConnectionDataPassError);
        }
        TransmitingData.clear();
   		SuspendThread(writer);
  	}
}
//---------------------------------------------------------------------------
//функция открытия и инициализации порта
void TComConnection::COMOpen(int _ComNumber, int _BaudRate)
{
	String TempPortName;     	 //имя порта (например, "COM1", "COM2" и т.д.)
    DCB dcb;                	//структура для общей инициализации порта DCB
    COMMTIMEOUTS Timeouts;  	//структура для установки таймаутов

 	TempPortName = "COM" + IntToStr(_ComNumber);	//получить имя выбранного порта

    COMport = CreateFile((const char*)TempPortName.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if(COMport == INVALID_HANDLE_VALUE)            //если ошибка открытия порта
    {
    	ConnectionErrorTrigger(this, ComConnectionOpenError);
    	return;
    }

 	//инициализация порта

 	dcb.DCBlength = sizeof(DCB); 	//в первое поле структуры DCB необходимо занести её длину, она будет использоваться функциями настройки порта для контроля корректности структуры

    //считать структуру DCB из порта
    if(!GetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  	{
    	COMClose();
       	ConnectionErrorTrigger(this, ComConnectionOpenError);
       	return;
  	}

 	//инициализация структуры DCB
 	dcb.BaudRate = _BaudRate;       					   //задаём скорость передачи
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
   		ConnectionErrorTrigger(this, ComConnectionOpenError);
   		return;
  	}

 	//установить таймауты
 	Timeouts.ReadIntervalTimeout = 0;	 			//таймаут между двумя символами
 	Timeouts.ReadTotalTimeoutMultiplier = 0;	//общий таймаут операции чтения
 	Timeouts.ReadTotalTimeoutConstant = 0;         //константа для общего таймаута операции чтения
 	Timeouts.WriteTotalTimeoutMultiplier = 0;      //общий таймаут операции записи
 	Timeouts.WriteTotalTimeoutConstant = 0;        //константа для общего таймаута операции записи

 	//записать структуру таймаутов в порт
 	if(!SetCommTimeouts(COMport, &Timeouts))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  	{
   		COMClose();
   		ConnectionErrorTrigger(this, ComConnectionOpenError);
   		return;
  	}

 	//установить размеры очередей приёма и передачи
 	SetupComm(COMport, 2000, 2000);

 	PurgeComm(COMport, PURGE_RXCLEAR);	//очистить принимающий буфер порта

 	reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//создаём поток чтения, который сразу начнёт выполняться (предпоследний параметр = 0)
 	writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//создаём поток записи в остановленном состоянии (предпоследний параметр = CREATE_SUSPENDED)
}
//---------------------------------------------------------------------------
void TComConnection::COMClose()
{
	if(writer)		//если поток записи работает, завершить его; проверка if(writer) обязательна, иначе возникают ошибки
  	{
        TerminateThread(writer,0);
        CloseHandle(overlappedwr.hEvent);	//нужно закрыть объект-событие
        CloseHandle(writer);
  	}

 	if(reader)		   //если поток чтения работает, завершить его; проверка if(reader) обязательна, иначе возникают ошибки
  	{
        TerminateThread(reader,0);
        CloseHandle(overlapped.hEvent);	//нужно закрыть объект-событие
        CloseHandle(reader);
  	}

    CloseHandle(COMport);                  //закрыть порт
    COMport = NULL;				//обнулить переменную для дескриптора порта

}
//---------------------------------------------------------------------------

