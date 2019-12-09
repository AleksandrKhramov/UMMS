//---------------------------------------------------------------------------

#ifndef UDPInterconnectionUIClassH
#define UDPInterconnectionUIClassH

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
class TUDPInterconnectionUI : TUDPInterconnectionAbstract
{
private:
public:

    void __fastcall UDPReadTrigger(TIdUDPListenerThread *AThread, const TIdBytes AData, TIdSocketHandle *ABinding);
    void SendBytes(const TIdBytes AData);
    void SendBytesVector(std::vector<unsigned char>);

    TUDPInterconnectionUI();
    ~TUDPInterconnectionUI();
};
//---------------------------------------------------------------------------
#endif
