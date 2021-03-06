//---------------------------------------------------------------------------
#include "UDPInterconnectionUIClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TUDPInterconnectionUI::TUDPInterconnectionUI()
	: TUDPInterconnectionAbstract()
{
    TRegistryUIPort *RegistryUIPort = new TRegistryUIPort;
    UDPClient->Port = (RegistryUIPort->ReadPort() == -1 ? 12001 : RegistryUIPort->ReadPort());
    delete RegistryUIPort;

    TRegistryServicePort *RegistryServicePort = new TRegistryServicePort;
    UDPServer->Bindings->Add();
    UDPServer->Bindings->Items[0]->IP = "0.0.0.0";
    UDPServer->Bindings->Items[0]->Port = (RegistryServicePort->ReadPort() == -1 ? 12000 : RegistryServicePort->ReadPort());
    UDPServer->DefaultPort = (RegistryServicePort->ReadPort() == -1 ? 12000 : RegistryServicePort->ReadPort());

    while(!UDPServer->Active)
    {
        try
        {
            UDPServer->Active = true;
        }
        catch(...)
        {
        	++UDPServer->Bindings->Items[0]->Port;
        }
    }
    UDPServer->OnUDPRead = UDPReadTrigger;
    RegistryServicePort->UpdatePort(UDPServer->Bindings->Items[0]->Port);
    delete RegistryServicePort;
}
//---------------------------------------------------------------------------
//Деструктор
TUDPInterconnectionUI::~TUDPInterconnectionUI()
{

}
//---------------------------------------------------------------------------
void __fastcall TUDPInterconnectionUI::UDPReadTrigger(TIdUDPListenerThread *AThread, const TIdBytes AData, TIdSocketHandle *ABinding)
{

    //Here'll handing code of AData


    /* Working test
	UDPClient->Host = "127.0.0.1";
	UDPClient->SendBuffer(AData);
    */
}
//---------------------------------------------------------------------------
void TUDPInterconnectionUI::SendBytes(const TIdBytes AData)
{
	UDPClient->Host = "127.0.0.1";
	UDPClient->SendBuffer(AData);
}
//---------------------------------------------------------------------------
void TUDPInterconnectionUI::SendBytesVector(std::vector<unsigned char> Data)
{
	TIdBytes AData;
    AData.set_length(Data.size());
    for(int i = 0; i < Data.size(); ++i)
    	AData[i] = Data[i];

    SendBytes(AData);
}
