//---------------------------------------------------------------------------
#include "RegistryUIPortClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор и коснтруктор базового класса
TRegistryUIPort::TRegistryUIPort()
	: TRegistryAbstract("SOFTWARE\\UMMS\\Local_UDP_Server")
{
	if(!Registry->ValueExists("UDPPortUI"))
    	Registry->WriteInteger("UDPPortUI", -1);
}
//---------------------------------------------------------------------------
//Деструктор
TRegistryUIPort::~TRegistryUIPort()
{

}
//---------------------------------------------------------------------------
void TRegistryUIPort::UpdatePort(int PortNumber)
{
	Registry->WriteInteger("UDPPortUI", PortNumber);
}
//---------------------------------------------------------------------------
int TRegistryUIPort::ReadPort()
{
    return Registry->ReadInteger("UDPPortUI");
}
