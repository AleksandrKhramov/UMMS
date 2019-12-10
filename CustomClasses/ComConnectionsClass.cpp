//---------------------------------------------------------------------------
#include "ComConnectionsClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TComConnections::TComConnections(TComponent* _Owner)
{
	DataReadyForSendingTrigger = NULL;
	Owner = _Owner;
}
//---------------------------------------------------------------------------
//����������
TComConnections::~TComConnections()
{
	for(int i = 0; i < ComConnections.size(); ++i)
		ComConnections[i]->~TComConnection();
    ComConnections.clear();
    ComConnections.~vector();
}
//---------------------------------------------------------------------------
void TComConnections::DataReadyTrigger(TComConnection *ComConnection, std::vector<byte> Data)
{
    Data.insert(Data.begin(), ComConnection->ComNumber);
    Data.insert(Data.begin(), SendData);
    Data.insert(Data.begin(), 0);         					//������������� Com-�����
	DataReadyForSendingTrigger(Data);
}
//---------------------------------------------------------------------------
void TComConnections::ConnectionErrorTrigger(int ErrorNumber)
{

}
//---------------------------------------------------------------------------
void TComConnections::HandingDataTrigger(std::vector<byte> Data)
{
	if(Data.size() == 0)
    	return;

    switch(Data[0])          								//��� ������ ��� ������
    {
    	case SetPattern :                                            //������ ������� ������
            Data.erase(Data.begin());
            AddPattern(Data);
            return;
        	break;

        case SendData :                                            //�������� ������
        	Data.erase(Data.begin());
        	if(Data[0] <= ComConnections.size())				//���� ����� Com - �����, �������� ������������� ������ ����������
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
void TComConnections::AddPattern(std::vector<byte> Pattern)
{
	Patterns.push_back(Pattern);
}
//---------------------------------------------------------------------------
void TComConnections::SearchDevices()
{
    if(!IsAllActiveConnectionsExists() && IsComListUpdated())
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

	if(ComNameList->Strings == TempComNames->Strings)
    	if(ComPortList->Strings == TempComPorts->Strings)
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
}
//---------------------------------------------------------------------------
void TComConnections::RemoveNonexistentConnections()
{
	TRegistryComPorts *RegistryComPorts = new TRegistryComPorts;
	TStringList *TempComNames = RegistryComPorts->GetComNames();
    TStringList *TempComPorts = RegistryComPorts->GetComPorts();
    
	for (int i = 0; i < ComConnections.size(); ++i) 
    {
    	if(-1 == TempComNames->IndexOf(ComConnections[i]->ComName))
        {
        	if(TempComNames->IndexOf(ComConnections[i]->ComName) == TempComPorts->IndexOf(ComConnections[i]->ComPort))
            {

            }
        }
        else
        {
        	ComConnections.erase(ComConnections.begin() + i--); 
        }	    
    }
    
	delete RegistryComPorts;	
}
//---------------------------------------------------------------------------
void TComConnections::AddNewConnections()
{

}
//---------------------------------------------------------------------------
