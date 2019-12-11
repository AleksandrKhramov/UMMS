//---------------------------------------------------------------------------
#include "ComConnectionsClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TComConnections::TComConnections(TComponent* _Owner)
{
	DataReadyForSendingTrigger = NULL;
	Owner = _Owner;
    IteratorByPatterns = new TIteratorByPatterns;
}
//---------------------------------------------------------------------------
//Деструктор
TComConnections::~TComConnections()
{
	for(int i = 0; i < ComConnections.size(); ++i)
		ComConnections[i]->~TComConnection();
    ComConnections.clear();
    ComConnections.~vector();

    delete IteratorByPatterns;
}
//---------------------------------------------------------------------------
void TComConnections::DataReadyTrigger(TComConnection *ComConnection, std::vector<byte> Data)
{
    if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
    {
        	//Connection correnspond to patterns
            /*
            	Handing of corresponding
            */
    	return;
    }
    Data.insert(Data.begin(), ComConnection->ComNumber);
    Data.insert(Data.begin(), SendData);
    Data.insert(Data.begin(), 0);         					//Идентификатор соединения (в данном случае Com-порта)
	DataReadyForSendingTrigger(Data);
}
//---------------------------------------------------------------------------
void TComConnections::ConnectionErrorTrigger(TComConnection *ComConnection, int ErrorNumber)
{
	int Index = IndexOfComConnection(ComConnection);
	switch(ErrorNumber)
    {
        case ComConnectionOpenError :
            if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
            	IteratorByPatterns->RemoveConnection(ComConnection);

            if (-1 != Index)
            {
                ComConnection->~TComConnection();
                ComConnections.erase(ComConnections.begin() + Index);
            }
        	break;
        case ComConnectionDataPassError :

        	if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
            {
            	if(!IteratorByPatterns->NextPatternForConnection(ComConnection))
                	;//Connection dont correnspond to patterns

            }
        	break;
        default :
        	break;
    }

}
//---------------------------------------------------------------------------
void TComConnections::HandingDataTrigger(std::vector<byte> Data)
{
	if(Data.size() == 0)
    	return;

    switch(Data[0])          								//Тип задачи для портов
    {
    	case SetPattern :                                            //Задать паттерн поиска
            Data.erase(Data.begin());
            IteratorByPatterns->AddPattern(Data);
            return;
        	break;

        case SendData :                                            //Передать данные
        	Data.erase(Data.begin());
        	if(Data[0] <= ComConnections.size())				//Если номер Com - порта, которому предназначены данные существует
            {
                int ComNumber = Data[0];
                Data.erase(Data.begin());
                ComConnections[ComNumber]->SendData(Data);
            }
            return;
        	break;

        default :
        	break;
    }
}
//---------------------------------------------------------------------------
void TComConnections::SearchDevices()
{
    if(!IsAllActiveConnectionsExists() || IsComListUpdated())
    {
     	UpdateActiveConnections();
    }
}
//---------------------------------------------------------------------------
bool TComConnections::IsAllActiveConnectionsExists()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();

	for(int i = 0; i < ComConnections.size(); ++i)
    {
		if(-1 == TempComNames->IndexOf(ComConnections[i]->ComName))
        {
            delete RegistryComPorts;
            return false;
        }
    }

    delete RegistryComPorts;
    return true;
}
//---------------------------------------------------------------------------
bool TComConnections::IsComListUpdated()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();
    TStringList *TempComPorts = RegistryComPorts->GetComPorts();

	if(IsListsIdentical(ComNameList, TempComNames))
    	if(IsListsIdentical(ComPortList, TempComPorts))
        {
        	delete RegistryComPorts;
       		return false;
        }

    delete RegistryComPorts;
    return true;
}
//---------------------------------------------------------------------------
void TComConnections::UpdateActiveConnections()
{
	RemoveNonexistentConnections();
    AddNewConnections();
    UpdateComLists();
}
//---------------------------------------------------------------------------
void TComConnections::RemoveNonexistentConnections()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();
    TStringList *TempComPorts = RegistryComPorts->GetComPorts();
    
	for (int i = 0; i < ComConnections.size(); ++i) 
    {
    	if((-1 == TempComNames->IndexOf(ComConnections[i]->ComName)) ||
        	(TempComNames->IndexOf(ComConnections[i]->ComName) != TempComPorts->IndexOf(IntToStr(ComConnections[i]->ComNumber))))
        {
        	ComNameList->Delete(ComNameList->IndexOf(ComConnections[i]->ComName));
            ComPortList->Delete(ComPortList->IndexOf(IntToStr(ComConnections[i]->ComNumber)));
            
        	NotifyDeviceDeleted(ComConnections[i]->ComNumber);
            
            ComConnections[i]->~TComConnection();
        	ComConnections.erase(ComConnections.begin() + i); 
                
            --i; 
        }	    
    }
    
	delete RegistryComPorts;	
}
//---------------------------------------------------------------------------
void TComConnections::AddNewConnections()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();
    TStringList *TempComPorts = RegistryComPorts->GetComPorts();

	for (int i = 0; i < TempComNames->Count; ++i)
    {
    	try
        {
            if(!IsComPortExists(StrToInt(TempComPorts->Strings[i])))
            {
                TComConnection *TempComConnection = new TComConnection(Owner, TempComNames->Strings[i], TempComPorts->Strings[i].ToInt(), DataReadyTrigger, ConnectionErrorTrigger);
                ComConnections.push_back(TempComConnection);
                IteratorByPatterns->AddConnectionOnIterating(TempComConnection);
            }
        }
        catch(...)
        {
        	//Handing Error of creating Connection
        }
    }

	delete RegistryComPorts;
}
//---------------------------------------------------------------------------
void TComConnections::NotifyDeviceDeleted(int ComNumber)
{

}
//---------------------------------------------------------------------------
void TComConnections::UpdateComLists()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();
    TStringList *TempComPorts = RegistryComPorts->GetComPorts();

    ComNameList->Clear();
    ComPortList->Clear();

    for (int i = 0; i < TempComNames->Count; ++i)
    {
    	ComNameList->Add(TempComNames->Strings[i]);
    }

    for (int i = 0; i < TempComPorts->Count; ++i)
    {
    	ComPortList->Add(TempComPorts->Strings[i]);
    }

	delete RegistryComPorts;
}
//---------------------------------------------------------------------------
bool TComConnections::IsComPortExists(int ComNumber)
{
	for (int i = 0; i < ComConnections.size(); ++i)
    {
     	if(ComConnections[i]->ComNumber == ComNumber)
        	return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool TComConnections::IsListsIdentical(TStringList *List1, TStringList *List2)
{
	if(List1->Count != List2->Count)
    	return false;

    for (int i = 0; i < List1->Count; ++i)
    {
    	if(List1->Strings[i] != List2->Strings[i])
        	return false;
    }

    return true;
}
//---------------------------------------------------------------------------
int TComConnections::IndexOfComConnection(TComConnection *ComConnection)
{
	for (int i = 0; i < ComConnections.size(); ++i)
    {
   		if(ComConnections[i] == ComConnection)
        	return i;
    }
    return -1;
}

