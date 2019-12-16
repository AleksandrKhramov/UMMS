//---------------------------------------------------------------------------
#include "RegistryComPortsClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//Конструктор и коснтруктор базового класса
TRegistryComPorts::TRegistryComPorts()
	: TRegistryAbstract("HARDWARE\\DEVICEMAP\\SERIALCOMM")
{
	ComNameList = new TStringList;
    ComPortList = new TStringList;
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

    Registry->GetValueNames(ComNameList);
    return ComNameList;
}
//---------------------------------------------------------------------------
TStringList *TRegistryComPorts::GetComPorts()
{
	GetComNames();

    for(int i = 0; i < ComNameList->Count ; i++)
    {
        ComPortList->Add(Registry->ReadString(ComNameList->Strings[i]).SubString(4, ComNameList->Strings[i].Length() - 3));
    }
    return ComPortList;
}
//---------------------------------------------------------------------------
std::pair<TStringList*, TStringList*> TRegistryComPorts::GetComNamesAndPorts()
{
    return std::make_pair<TStringList*, TStringList*>(GetComNames(), GetComPorts());
}
//---------------------------------------------------------------------------
