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
    //TRegistryComPorts
}
