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

    ComNumber = _ComNumber;
    ComName = _ComName;

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
//функция открытия и инициализации порта
void TComConnection::COMOpen(int _ComNumber, int _BaudRate)
{

	String TempPortName;     	 //имя порта (например, "COM1", "COM2" и т.д.)
    DCB dcb;                	//структура для общей инициализации порта DCB
    COMMTIMEOUTS Timeouts;  	//структура для установки таймаутов

 	TempPortName = "\\\\.\\COM" + IntToStr(_ComNumber);	//получить имя выбранного порта

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
}
//---------------------------------------------------------------------------
void TComConnection::COMClose()
{
    CloseHandle(COMport);                  //закрыть порт
    COMport = NULL;				//обнулить переменную для дескриптора порта
}
//---------------------------------------------------------------------------
void TComConnection::ConnectionTest()
{
    COMOpen(ComNumber);
	COMClose();
}
