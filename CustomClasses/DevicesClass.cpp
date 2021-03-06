//---------------------------------------------------------------------------
#include "DevicesClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TDevices::TDevices()
{
	MS3071Interface = new TMS3071Interface;
    UDPInterconnection = new TUDPInterconnectionUI;

    std::vector< std::vector<unsigned char> > SearchPatterns =  MS3071Interface->GetPatterns();

    //��� ����� ������� ��������� �����
    //-----------------------------------------------
    std::vector<unsigned char> UDPParsMessage;

    for(int PatternNumber = 0; PatternNumber < SearchPatterns.size(); ++PatternNumber)
    {

        UDPParsMessage.push_back(0);
        UDPParsMessage.push_back(0);

        for(int i = 0; i < SearchPatterns[PatternNumber].size(); ++i)
        	UDPParsMessage.push_back(SearchPatterns[PatternNumber][i]);

     	UDPInterconnection->SendBytesVector(UDPParsMessage);

        UDPParsMessage.clear();
    }
    //-----------------------------------------------
}
//---------------------------------------------------------------------------
//����������
TDevices::~TDevices()
{
	delete MS3071Interface;
    delete UDPInterconnection;
}
//---------------------------------------------------------------------------