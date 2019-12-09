//---------------------------------------------------------------------------
#include "RegistryComPortsClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//Конструктор и коснтруктор базового класса
TRegistryComPorts::TRegistryComPorts()
	: TRegistryAbstract("HARDWARE\\DEVICEMAP\\SERIALCOMM")
{

}
//---------------------------------------------------------------------------
//Деструктор
TRegistryComPorts::~TRegistryComPorts()
{
	delete ComNameList;
    delete ComPortList;
}
//---------------------------------------------------------------------------
TStringList *TRegistryComPorts::GetComNames()
{
	ComNameList = new TStringList;
    Registry->GetValueNames(ComNameList);
    return ComNameList;
}
//---------------------------------------------------------------------------
TStringList *TRegistryComPorts::GetComPorts()
{
	GetComNames();
    ComPortList = new TStringList;
    for(int i = 0; i < ComNameList->Count ; i++)
    {
        ComPortList->Add(Registry->ReadString(ComNameList->Strings[i]).SubString(4, 1));
    }
    return ComPortList;
}
//---------------------------------------------------------------------------
std::pair<TStringList*, TStringList*> TRegistryComPorts::GetComNamesAndPorts()
{
    return std::make_pair<TStringList*, TStringList*>(GetComNames(), GetComPorts());
}
//---------------------------------------------------------------------------
