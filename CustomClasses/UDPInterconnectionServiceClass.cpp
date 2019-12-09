//---------------------------------------------------------------------------
#include "UDPInterconnectionServiceClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TUDPInterconnectionService::TUDPInterconnectionService()
	: TUDPInterconnectionAbstract()
{
    TRegistryServicePort *RegistryServicePort = new TRegistryServicePort;
    UDPClient->Port = (RegistryServicePort->ReadPort() == -1 ? 12000 : RegistryServicePort->ReadPort());
    delete RegistryServicePort;

    TRegistryUIPort *RegistryUIPort = new TRegistryUIPort;
    UDPServer->Bindings->Add();
    UDPServer->Bindings->Items[0]->IP = "0.0.0.0";
    UDPServer->Bindings->Items[0]->Port = (RegistryUIPort->ReadPort() == -1 ? 12001 : RegistryUIPort->ReadPort());
    UDPServer->DefaultPort = (RegistryUIPort->ReadPort() == -1 ? 12001 : RegistryUIPort->ReadPort()); 				//����� ���������� ������������� ������

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
    RegistryUIPort->UpdatePort(UDPServer->Bindings->Items[0]->Port);
    delete RegistryUIPort;
}
//---------------------------------------------------------------------------
//����������
TUDPInterconnectionService::~TUDPInterconnectionService()
{

}
//---------------------------------------------------------------------------
void __fastcall TUDPInterconnectionService::UDPReadTrigger(TIdUDPListenerThread *AThread, const TIdBytes AData, TIdSocketHandle *ABinding)
{
	if(AData.Length == 0)								//���� ������ ���
    	return;

	std::vector<byte> Data;
    for(int i = 1; i < AData.Length; ++i)
    	Data.push_back(AData[i]);

	switch(AData[0])         							//��� ����������
    {
     	case 0 :                                    	//Com - ����
			DataForComPortTrigger(Data);
        	break;
        default :
        	break;
    }
}
//---------------------------------------------------------------------------
void TUDPInterconnectionService::SendBytes(const TIdBytes AData)
{
	UDPClient->Host = "127.0.0.1";
	UDPClient->SendBuffer(AData);
}
//---------------------------------------------------------------------------
void TUDPInterconnectionService::DataReadyForSendingTrigger(std::vector<byte> Data)
{
    TIdBytes AData;
    AData.set_length(Data.size());
    for (int i = 0; i < Data.size(); i++) 
    {
    	AData[i] = Data[i];    
    }
	SendBytes(AData);
}
