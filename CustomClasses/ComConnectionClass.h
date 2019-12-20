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

    void COMOpen(int _ComNumber, int _BaudRate = 9600);
    void COMClose(void);

    void (__closure *DataReadyTrigger)(TComConnection *, std::vector<byte>);
    void (__closure *ConnectionErrorTrigger)(TComConnection *, int ErrorNumber);

    void SendData(std::vector<byte> Data);
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
