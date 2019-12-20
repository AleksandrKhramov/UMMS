//---------------------------------------------------------------------------
#ifndef ComConnectionClassH
#define ComConnectionClassH
//---------------------------------------------------------------------------
//							  Built headers
#include <vector>
#include <System.hpp>
#include <System.SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ComConnectionErrors.cpp"
//---------------------------------------------------------------------------
class TComConnection
{
private:
public:

    String ComName;
    int ComNumber;
    int ExpectationDelay;

    HANDLE COMport;				//дескриптор порта
    //static HANDLE reader;				//дескриптор потока чтения из порта
	//static HANDLE writer;				//дескриптор потока записи в порт

    void COMOpen(int _ComNumber, int _BaudRate = 9600);
    void COMClose(void);

    //static DWORD WINAPI ReadThread(LPVOID);
	//static DWORD WINAPI WriteThread(LPVOID);

    void (__closure *DataReadyTrigger)(TComConnection *, std::vector<byte>);
    void (__closure *ConnectionErrorTrigger)(TComConnection *, int ErrorNumber);

    OVERLAPPED overlapped;		//будем использовать для операций чтения (см. поток ReadThread)
	OVERLAPPED overlappedwr;       	//будем использовать для операций записи (см. поток WriteThread)
    void SendData(std::vector<byte> Data);
    void Write(std::vector<byte> Data);
    void WaitAnswer();

    TComConnection( String ComName,
                    int ComNumber,
                    int ExpectationDelay,
    				void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                    void (__closure *_ConnectionErrorTrigger)(TComConnection *, int ErrorNumber),
     				int BaudRate = 9600);

    ~TComConnection();                                                            	//Деструктор
};
//---------------------------------------------------------------------------
#endif
