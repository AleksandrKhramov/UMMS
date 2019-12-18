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
    std::vector<byte> TransmittingData;

    HANDLE COMport;				//дескриптор порта
    HANDLE reader;				//дескриптор потока чтения из порта
	HANDLE writer;				//дескриптор потока записи в порт

    void COMOpen(int _ComNumber, int _BaudRate);
    void COMClose(void);

    DWORD WINAPI ReadThread(LPVOID);
	DWORD WINAPI WriteThread(LPVOID);

    void (__closure *DataReadyTrigger)(TComConnection *, std::vector<byte>);
    void (__closure *ConnectionErrorTrigger)(TComConnection *, int ErrorNumber);

    void SendData(std::vector<byte> Data);

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
