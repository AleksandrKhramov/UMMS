//---------------------------------------------------------------------------
#include "RegistryServicePortClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//����������� � ����������� �������� ������
TRegistryServicePort::TRegistryServicePort()
	: TRegistryAbstract("SOFTWARE\\UMMS\\Local_UDP_Server")
{
	if(!Registry->ValueExists("UDPPortService"))
    	Registry->WriteInteger("UDPPortService", -1);


}
//---------------------------------------------------------------------------
//����������
TRegistryServicePort::~TRegistryServicePort()
{

}
//---------------------------------------------------------------------------
void TRegistryServicePort::UpdatePort(int PortNumber)
{
	Registry->WriteInteger("UDPPortService", PortNumber);
}
//---------------------------------------------------------------------------
int TRegistryServicePort::ReadPort()
{
    return Registry->ReadInteger("UDPPortService");
}