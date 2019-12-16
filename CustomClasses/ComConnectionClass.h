//---------------------------------------------------------------------------
#ifndef ComConnectionClassH
#define ComConnectionClassH
//---------------------------------------------------------------------------
//							  Built headers
#include "AdPort.hpp"
#include "OoMisc.hpp"
#include <vector>
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
    TApdComPort* ComPort;
    TTimer* TimeoutTimer;
    TTimer* ExpectationTimer;
    std::vector<byte> RecievedData;
    String ComName;
    int ComNumber;

    void __fastcall ComPortOnTriggerAvail(TObject *CP, WORD Count);
    void __fastcall TimoutTimerOnTimer(TObject *Sender);
    void __fastcall ExpectationTimerOnTimer(TObject *Sender);

    void (__closure *DataReadyTrigger)(TComConnection *, std::vector<byte>);
    void (__closure *ConnectionErrorTrigger)(TComConnection *, int ErrorNumber);

    void SendData(std::vector<byte> Data);

    TComConnection(TComponent* Owner,                                   			//Конструктор
                    String ComName,
                    int ComNumber,
                    int ExpectationDelay,
    				void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                    void (__closure *_ConnectionErrorTrigger)(TComConnection *, int ErrorNumber),
     				int BaudRate = 9600);

    ~TComConnection();                                                            	//Деструктор
};
//---------------------------------------------------------------------------
#endif
