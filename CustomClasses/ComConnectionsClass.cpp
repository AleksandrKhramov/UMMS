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

    ComNameList = new TStringList();
    ComPortList = new TStringList();

    SearchingTimer = new TTimer(Owner);
    SearchingTimer->Enabled = false;
    SearchingTimer->Interval = 500;
    SearchingTimer->OnTimer = SearchingTimerOnTimer;
    SearchingTimer->Name = "SearchingTimerForComPort";
    SearchingTimer->Enabled = true;
}
//---------------------------------------------------------------------------
//Деструктор
TComConnections::~TComConnections()
{
	for(int i = 0; i < ComConnections.size(); ++i)
		ComConnections[i]->~TComConnection();
    ComConnections.clear();
    ComConnections.~vector();

    delete SearchingTimer;
    delete IteratorByPatterns;

    delete ComNameList;
    delete ComPortList;
}
//---------------------------------------------------------------------------
//***************Внутренние и внешние обработчики событий********************
void TComConnections::DataReadyTrigger(TComConnection *ComConnection, std::vector<byte> Data)
{
    if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
    {
        NotifyDeviceConnected(ComConnection->ComNumber);
        IteratorByPatterns->RemoveConnection(ComConnection);
    	return;
    }

    ExternalSend(DataHandingSendData, ComConnection->ComNumber, Data);
}
//---------------------------------------------------------------------------
void TComConnections::ConnectionErrorTrigger(TComConnection *ComConnection, int ErrorNumber)
{
   // ExternalSend(ErrorNumber, ComConnection->ComNumber);
	int Index = IndexOfComConnection(ComConnection);
	switch(ErrorNumber)
    {
        case ComConnectionOpenError :
        case ComConnectionOpenInvalidHandleError :
            if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
            	IteratorByPatterns->RemoveConnection(ComConnection);

            if(IteratorByPatterns->IsReiteratedConnection(ComConnection))
            {
                if (-1 != Index)
                	NotifyDeviceDisconnected(ComConnection->ComNumber);
                RemoveConnectionFully(ComConnection);
                break;
            }

            if (-1 != Index)
            	RemoveConnection(ComConnection);
        	break;
        case ComConnectionDataPassError :
            if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
            {
            	if(!IteratorByPatterns->NextPatternForConnection(ComConnection))
                {
                	IteratorByPatterns->RemoveConnection(ComConnection);
                    if(IteratorByPatterns->IsReiteratedConnection(ComConnection))
                    {
                    	if (-1 != Index)
                			NotifyDeviceDisconnected(ComConnection->ComNumber);
                        RemoveConnectionFully(ComConnection);
                        break;
                    }

                    if (-1 != Index)
                        RemoveConnection(ComConnection);
                }
            }
        	break;
        case ComConnectionDataPassExpectationError :
        	if(IteratorByPatterns->IsConnectionOnIterating(ComConnection))
            {
            	if(!IteratorByPatterns->NextPatternForConnection(ComConnection))
                {
                	if(IteratorByPatterns->IsReiteratedConnection(ComConnection))
                    {
                    	if (-1 != Index)
                			NotifyDeviceDisconnected(ComConnection->ComNumber);
                        RemoveConnectionFully(ComConnection);
                        break;
                    }

                    if (-1 != Index)
                        RemoveConnection(ComConnection);
                }
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

    switch(Data[0])
    {
    	case DataHandingSetPattern :
            Data.erase(Data.begin());
            IteratorByPatterns->AddPattern(Data);
            IteratorByPatterns->ClearConnections();


            for (int ComIndex = 0; ComIndex < ComConnections.size(); ++ComIndex)
            {
            	IteratorByPatterns->AddConnectionOnIterating(ComConnections[ComIndex]);
            }
            return;
        	break;

        case DataHandingSendData :
        	Data.erase(Data.begin());
        	if(Data[0] <= ComConnections.size())
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
//***********************Функции алгоритма поиска****************************
void TComConnections::SearchDevices()
{
    if(!IsAllActiveConnectionsExists() || IsComListUpdated())
    {
   		RemoveNonexistentConnections();
        AddNewConnections();
        UpdateComLists();
    }
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
           (TempComNames->IndexOf(ComConnections[i]->ComName) != TempComPorts->IndexOf(IntToStr(ComConnections[i]->ComNumber))) )
        {
        	if(IteratorByPatterns->IsConnectionOnIterating(ComConnections[i]))
            	IteratorByPatterns->RemoveConnection(ComConnections[i]);

            if(-1 != ComNameList->IndexOf(ComConnections[i]->ComName))
        		ComNameList->Delete(ComNameList->IndexOf(ComConnections[i]->ComName));

            if(-1 != ComPortList->IndexOf(IntToStr(ComConnections[i]->ComNumber)))
            	ComPortList->Delete(ComPortList->IndexOf(IntToStr(ComConnections[i]->ComNumber)));

        	NotifyDeviceDisconnected(ComConnections[i]->ComNumber);

            RemoveConnection(ComConnections[i]);

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
            if(!IsComPortExists(TempComPorts->Strings[i].ToInt()))
            {
            	TComConnection *TempComConnection = new TComConnection(TempComNames->Strings[i], TempComPorts->Strings[i].ToInt(), 200, DataReadyTrigger, ConnectionErrorTrigger);

                if(TempComConnection != NULL)
                {
                    ComConnections.push_back(TempComConnection);
                    IteratorByPatterns->AddConnectionOnIterating(TempComConnection);
                }
            }
            else
            {
               	if(!IteratorByPatterns->IsConnectionOnIterating(ComConnectionOfComNumber(TempComPorts->Strings[i].ToInt())))
                 	IteratorByPatterns->AddConnectionOnIterating(ComConnectionOfComNumber(TempComPorts->Strings[i].ToInt()), true);
            }
        }
        catch(...)
        {

        }
    }

	delete RegistryComPorts;
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
void TComConnections::RemoveConnection(TComConnection *ComConnection)
{
	int ConnectionIndex = IndexOfComConnection(ComConnection);
    if (ConnectionIndex >= 0)
    {
     	ComConnections[ConnectionIndex]->~TComConnection();
        ComConnections.erase(ComConnections.begin() + ConnectionIndex);
    }
}
//---------------------------------------------------------------------------
void TComConnections::RemoveConnectionFully(TComConnection *ComConnection)
{
    if(-1 != ComNameList->IndexOf(ComConnection->ComName))
    	ComNameList->Delete(ComNameList->IndexOf(ComConnection->ComName));

    if(-1 != ComPortList->IndexOf(IntToStr(ComConnection->ComNumber)))
    	ComPortList->Delete(ComPortList->IndexOf(IntToStr(ComConnection->ComNumber)));

    RemoveConnection(ComConnection);
}
//---------------------------------------------------------------------------
void __fastcall TComConnections::SearchingTimerOnTimer(TObject *Sender)
{
    SearchingTimer->Enabled = false;
    SearchDevices();
    SearchingTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void TComConnections::ExternalSend(int DataHanding, int ComNumber, std::vector<byte> Data)
{
    std::vector<byte> SendData;
    if((std::vector<byte>)NULL != Data)
    	SendData = Data;

    SendData.insert(SendData.begin(), ComNumber);
    SendData.insert(SendData.begin(), DataHanding);
    SendData.insert(SendData.begin(), ComPortConnectionType);

    DataReadyForSendingTrigger(SendData);
}
//---------------------------------------------------------------------------
void TComConnections::NotifyDeviceDisconnected(int ComNumber)
{
    ExternalSend(DataHandingConnectionRemoved, ComNumber);
}
//---------------------------------------------------------------------------
void TComConnections::NotifyDeviceConnected(int ComNumber)
{
    ExternalSend(DataHandingNewConnection, ComNumber);
}
//---------------------------------------------------------------------------
void TComConnections::ExternalConnectionsUpdate(bool Full)
{
    SearchingTimer->Enabled = false;
    if(Full)
    {
        for(int i = 0; i < ComConnections.size(); ++i)
            ComConnections[i]->~TComConnection();

        ComConnections.clear();

        ComNameList->Clear();
        ComPortList->Clear();
    }
    SearchDevices();
    SearchingTimer->Enabled = true;
}
//---------------------------------------------------------------------------
//***************************Условные функции********************************
bool TComConnections::IsComPortExists(int ComNumber)
{
	for (int i = 0; i < ComConnections.size(); ++i)
     	if(ComConnections[i]->ComNumber == ComNumber)
        	return true;
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
bool TComConnections::IsAllActiveConnectionsExists()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComPorts = RegistryComPorts->GetComPorts();

	for(int i = 0; i < ComConnections.size(); ++i)
    {
		if(-1 == TempComPorts->IndexOf(IntToStr(ComConnections[i]->ComNumber)))
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
//************************Вспомогательные функции****************************
int TComConnections::IndexOfComConnection(TComConnection *ComConnection)
{
	for (int i = 0; i < ComConnections.size(); ++i)
    {
   		if(ComConnections[i] == ComConnection)
        	return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
TComConnection *TComConnections::ComConnectionOfComNumber(int ComNumber)
{
	for (int i = 0; i < ComConnections.size(); ++i)
    {
   		if(ComConnections[i]->ComNumber == ComNumber)
        	return ComConnections[i];
    }
    return NULL;
}
//---------------------------------------------------------------------------
