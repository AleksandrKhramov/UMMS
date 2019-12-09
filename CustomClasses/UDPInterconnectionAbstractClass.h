
#ifndef UDPInterconnectionAbstractClassH
#define UDPInterconnectionAbstractClassH

//---------------------------------------------------------------------------
//							  Built headers
//Common UDP headers
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdUDPBase.hpp>
//UDPServer headers
#include <IdSocketHandle.hpp>
#include <IdUDPServer.hpp>
//UDPClient headers
#include <IdUDPClient.hpp>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TUDPInterconnectionAbstract
{

private:
public:
	TIdUDPClient *UDPClient;
	TIdUDPServer *UDPServer;

    TUDPInterconnectionAbstract();
    virtual ~TUDPInterconnectionAbstract();
};
//---------------------------------------------------------------------------
#endif
