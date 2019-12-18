//---------------------------------------------------------------------------
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdPort"
#pragma link "OoMisc"
//---------------------------------------------------------------------------
//Конструктор
TComConnection::TComConnection(TComponent* Owner, String _ComName, int _ComNumber, int ExpectationDelay,
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

    ExpectationTimer = new TTimer(Owner);
    ExpectationTimer->Enabled = false;
    ExpectationTimer->Interval = ExpectationDelay;
    ExpectationTimer->OnTimer = ExpectationTimerOnTimer;
    ExpectationTimer->Name = "ExpectationTimerForComPort" + IntToStr(ComNumber);

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
        return;
    }

    ComPort->Open = false;
}
//---------------------------------------------------------------------------
//Деструктор
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
    	ComPort->PutBlock(Buf, Data.size()); 					//Отправляем данные
        ExpectationTimer->Enabled = true;
    }
    catch(...)
    {
    	ConnectionErrorTrigger(this, ComConnectionDataPassError);//Ошибка передачи
    }

    delete [] Buf;
}






//=============================================================================
//..................... объявления глобальных переменных ......................
//=============================================================================

#define BUFSIZE 255     //ёмкость буфера

unsigned char bufrd[BUFSIZE], bufwr[BUFSIZE]; //приёмный и передающий буферы

//---------------------------------------------------------------------------

HANDLE COMport;		//дескриптор порта

//структура OVERLAPPED необходима для асинхронных операций, при этом для операции чтения и записи нужно объявить разные структуры
//эти структуры необходимо объявить глобально, иначе программа не будет работать правильно
OVERLAPPED overlapped;		//будем использовать для операций чтения (см. поток ReadThread)
OVERLAPPED overlappedwr;       	//будем использовать для операций записи (см. поток WriteThread)

//---------------------------------------------------------------------------

int handle;             	//дескриптор для работы с файлом с помощью библиотеки <io.h>

//---------------------------------------------------------------------------

bool fl=0;	//флаг, указывающий на успешность операций записи (1 - успешно, 0 - не успешно)

unsigned long counter;	//счётчик принятых байтов, обнуляется при каждом открытии порта


//=============================================================================
//.............................. объявления функций ...........................
//=============================================================================

void COMOpen(void);             //открыть порт
void COMClose(void);            //закрыть порт


//=============================================================================
//.............................. объявления потоков ...........................
//=============================================================================

HANDLE reader;	//дескриптор потока чтения из порта
HANDLE writer;	//дескриптор потока записи в порт

DWORD WINAPI ReadThread(LPVOID);
DWORD WINAPI WriteThread(LPVOID);


//=============================================================================
//.............................. реализация потоков ...........................
//=============================================================================

//-----------------------------------------------------------------------------
//............................... поток ReadThead .............................
//-----------------------------------------------------------------------------

void ReadPrinting(void);

//---------------------------------------------------------------------------

//главная функция потока, реализует приём байтов из COM-порта
DWORD WINAPI ReadThread(LPVOID)
{
 COMSTAT comstat;		//структура текущего состояния порта, в данной программе используется для определения количества принятых в порт байтов
 DWORD btr, temp, mask, signal;	//переменная temp используется в качестве заглушки

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
        btr = comstat.cbInQue;                          	//и получить из неё количество принятых байтов
        if(btr)                         			//если действительно есть байты для чтения
        {
         ReadFile(COMport, bufrd, btr, &temp, &overlapped);     //прочитать байты из порта в буфер программы
         counter+=btr;                                          //увеличиваем счётчик байтов
         ReadPrinting();                      		//вызываем функцию для вывода данных на экран и в файл
        }
       }
    }
  }
}

//---------------------------------------------------------------------------

//выводим принятые байты на экран и в файл (если включено)
void ReadPrinting()
{
 Form1->Memo1->Lines->Add((char*)bufrd);	 //выводим принятую строку в Memo
 Form1->StatusBar1->Panels->Items[2]->Text = "Всего принято " + IntToStr(counter) + " байт";	//выводим счётчик в строке состояния

 if(Form1->CheckBox3->Checked == true)  //если включен режим вывода в файл
  {
   write(handle, bufrd, strlen(bufrd)); //записать в файл данные из приёмного буфера
  }
 memset(bufrd, 0, BUFSIZE);	        //очистить буфер (чтобы данные не накладывались друг на друга)
}

//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//............................... поток WriteThead ............................
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------

//главная функция потока, выполняет передачу байтов из буфера в COM-порт
DWORD WINAPI WriteThread(LPVOID)
{
 DWORD temp, signal;	//temp - переменная-заглушка

 overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL);   	  //создать событие
 while(1)
  {WriteFile(COMport, bufwr, strlen(bufwr), &temp, &overlappedwr);  //записать байты в порт (перекрываемая операция!)
   signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE);	  //приостановить поток, пока не завершится перекрываемая операция WriteFile

   if((signal == WAIT_OBJECT_0) && (GetOverlappedResult(COMport, &overlappedwr, &temp, true)))	//если операция завершилась успешно
     {
      Form1->StatusBar1->Panels->Items[0]->Text  = "Передача прошла успешно";    //вывести сообщение об этом в строке состояния
     }
   else {Form1->StatusBar1->Panels->Items[0]->Text  = "Ошибка передачи";} 	//иначе вывести в строке состояния сообщение об ошибке

   SuspendThread(writer);

  }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//кнопка "Передать"
void __fastcall TForm1::Button1Click(TObject *Sender)
{
 memset(bufwr,0,BUFSIZE);			//очистить программный передающий буфер, чтобы данные не накладывались друг на друга
 PurgeComm(COMport, PURGE_TXCLEAR);             //очистить передающий буфер порта
 strcpy(bufwr,Form1->Edit1->Text.c_str());      //занести в программный передающий буфер строку из Edit1

 ResumeThread(writer);               //активировать поток записи данных в порт

}
//---------------------------------------------------------------------------

//=============================================================================
//........................... реализации функций ..............................
//=============================================================================

//---------------------------------------------------------------------------

//функция открытия и инициализации порта
void COMOpen()
{
 String portname;     	 //имя порта (например, "COM1", "COM2" и т.д.)
 DCB dcb;                //структура для общей инициализации порта DCB
 COMMTIMEOUTS timeouts;  //структура для установки таймаутов

 portname = Form1->ComboBox1->Text;	//получить имя выбранного порта

 //открыть порт, для асинхронных операций обязательно нужно указать флаг FILE_FLAG_OVERLAPPED
 COMport = CreateFile(portname.c_str(),GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
 //здесь:
 // - portname.c_str() - имя порта в качестве имени файла, c_str() преобразует строку типа String в строку в виде массива типа char, иначе функция не примет
 // - GENERIC_READ | GENERIC_WRITE - доступ к порту на чтение/записть
 // - 0 - порт не может быть общедоступным (shared)
 // - NULL - дескриптор порта не наследуется, используется дескриптор безопасности по умолчанию
 // - OPEN_EXISTING - порт должен открываться как уже существующий файл
 // - FILE_FLAG_OVERLAPPED - этот флаг указывает на использование асинхронных операций
 // - NULL - указатель на файл шаблона не используется при работе с портами

 if(COMport == INVALID_HANDLE_VALUE)            //если ошибка открытия порта
  {
   Form1->SpeedButton1->Down = false;           //отжать кнопку
   Form1->StatusBar1->Panels->Items[0]->Text = "Не удалось открыть порт";       //вывести сообщение в строке состояния
   return;
  }

 //инициализация порта

 dcb.DCBlength = sizeof(DCB); 	//в первое поле структуры DCB необходимо занести её длину, она будет использоваться функциями настройки порта для контроля корректности структуры

 //считать структуру DCB из порта
 if(!GetCommState(COMport, &dcb))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Не удалось считать DCB";
   return;
  }

 //инициализация структуры DCB
 dcb.BaudRate = StrToInt(Form1->ComboBox2->Text);       //задаём скорость передачи 115200 бод
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
   Form1->StatusBar1->Panels->Items[0]->Text  = "Не удалось установить DCB";
   return;
  }

 //установить таймауты
 timeouts.ReadIntervalTimeout = 0;	 	//таймаут между двумя символами
 timeouts.ReadTotalTimeoutMultiplier = 0;	//общий таймаут операции чтения
 timeouts.ReadTotalTimeoutConstant = 0;         //константа для общего таймаута операции чтения
 timeouts.WriteTotalTimeoutMultiplier = 0;      //общий таймаут операции записи
 timeouts.WriteTotalTimeoutConstant = 0;        //константа для общего таймаута операции записи

 //записать структуру таймаутов в порт
 if(!SetCommTimeouts(COMport, &timeouts))	//если не удалось - закрыть порт и вывести сообщение об ошибке в строке состояния
  {
   COMClose();
   Form1->StatusBar1->Panels->Items[0]->Text  = "Не удалось установить тайм-ауты";
   return;
  }

 //установить размеры очередей приёма и передачи
 SetupComm(COMport,2000,2000);

 //создать или открыть существующий файл для записи принимаемых данных
 handle = open("test.txt", O_CREAT | O_APPEND | O_BINARY | O_WRONLY, S_IREAD | S_IWRITE);

 if(handle==-1)		//если произошла ошибка открытия файла
  {
   Form1->StatusBar1->Panels->Items[1]->Text = "Ошибка открытия файла";	//вывести сообщение об этом в командной строке
   Form1->Label6->Hide();                                               //спрятать надпись с именем файла
   Form1->CheckBox3->Checked = false;                                   //сбросить и отключить галочку
   Form1->CheckBox3->Enabled = false;
  }
 else { Form1->StatusBar1->Panels->Items[0]->Text = "Файл открыт успешно"; } //иначе вывести в строке состояния сообщение об успешном открытии файла

 PurgeComm(COMport, PURGE_RXCLEAR);	//очистить принимающий буфер порта

 reader = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);			//создаём поток чтения, который сразу начнёт выполняться (предпоследний параметр = 0)
 writer = CreateThread(NULL, 0, WriteThread, NULL, CREATE_SUSPENDED, NULL);	//создаём поток записи в остановленном состоянии (предпоследний параметр = CREATE_SUSPENDED)

}

//---------------------------------------------------------------------------

//функция закрытия порта
void COMClose()
{
  //Примечание: так как при прерывании потоков, созданных с помощью функций WinAPI, функцией TerminateThread
  //	      поток может быть прерван жёстко, в любом месте своего выполнения, то освобождать дескриптор
  //	      сигнального объекта-события, находящегося в структуре типа OVERLAPPED, связанной с потоком,
  //	      следует не внутри кода потока, а отдельно, после вызова функции TerminateThread.
  //	      После чего нужно освободить и сам дескриптор потока.

 if(writer)		//если поток записи работает, завершить его; проверка if(writer) обязательна, иначе возникают ошибки
  {TerminateThread(writer,0);
   CloseHandle(overlappedwr.hEvent);	//нужно закрыть объект-событие
   CloseHandle(writer);
  }
 if(reader)		   //если поток чтения работает, завершить его; проверка if(reader) обязательна, иначе возникают ошибки
  {TerminateThread(reader,0);
   CloseHandle(overlapped.hEvent);	//нужно закрыть объект-событие
   CloseHandle(reader);
  }

 CloseHandle(COMport);                  //закрыть порт
 COMport=0;				//обнулить переменную для дескриптора порта
 close(handle);				//закрыть файл, в который велась запись принимаемых данных
 handle=0;				//обнулить переменную для дескриптора файла

}

//---------------------------------------------------------------------------

