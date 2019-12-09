//---------------------------------------------------------------------------
#include "UDPInterconnectionAbstractClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TUDPInterconnectionAbstract::TUDPInterconnectionAbstract()
{

	UDPClient = new TIdUDPClient;
    UDPClient->Name = "UDPClient";
    UDPClient->Active = false;
    UDPClient->BoundIP = "";
    UDPClient->BoundPort = 0;
    UDPClient->BoundPortMax = 0;
    UDPClient->BoundPortMin = 0;
    UDPClient->BroadcastEnabled = true;
    UDPClient->BufferSize = 8192;
    UDPClient->Host = "";
    UDPClient->IPVersion = Id_IPv4;
    UDPClient->ReceiveTimeout = -2;
    UDPClient->ReuseSocket = rsOSDependent;
    UDPClient->Tag = 0;

	UDPServer = new TIdUDPServer;
    UDPServer->Name = "UDPServer";
    UDPServer->Active = false;
    UDPServer->BroadcastEnabled = true;
    UDPServer->BufferSize = 8192;
    UDPServer->IPVersion = Id_IPv4;
    UDPServer->ReuseSocket = rsOSDependent;
    UDPServer->Tag = 0;
    UDPServer->ThreadedEvent = true;
}
//---------------------------------------------------------------------------
//Деструктор
TUDPInterconnectionAbstract::~TUDPInterconnectionAbstract()
{
	delete UDPClient;
    delete UDPServer;
}

