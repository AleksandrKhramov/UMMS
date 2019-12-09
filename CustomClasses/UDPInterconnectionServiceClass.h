//---------------------------------------------------------------------------

#ifndef UDPInterconnectionServiceClassH
#define UDPInterconnectionServiceClassH

//---------------------------------------------------------------------------
//							  Built headers
#include <vector>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "UDPInterconnectionAbstractClass.h"
#include "RegistryUIPortClass.h"
#include "RegistryServicePortClass.h"
//---------------------------------------------------------------------------
class TUDPInterconnectionService : TUDPInterconnectionAbstract
{
private:
public:

    void __fastcall UDPReadTrigger(TIdUDPListenerThread *AThread, const TIdBytes AData, TIdSocketHandle *ABinding);
    void SendBytes(const TIdBytes AData);
    void (__closure *DataForComPortTrigger)(std::vector<byte> Data);
    void DataReadyForSendingTrigger(std::vector<byte> Data);

    TUDPInterconnectionService();
    ~TUDPInterconnectionService();
};
//---------------------------------------------------------------------------
#endif
