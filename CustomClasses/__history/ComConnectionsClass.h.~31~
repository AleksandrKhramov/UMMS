//---------------------------------------------------------------------------
#ifndef ComConnectionsClassH
#define ComConnectionsClassH
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ComConnectionClass.h"
#include "RegistryComPortsClass.h"
//---------------------------------------------------------------------------
class TComConnections
{
	enum ExternalDataHanding
    {
        SetPattern,
        SendData
    };

private:
public:
	TComponent* Owner;
	std::vector<TComConnection *> ComConnections;
    std::vector< std::vector<byte> > Patterns;

    void SearchDevices();
    void AddPattern(std::vector<byte>);
	void DataReadyTrigger(TComConnection *, std::vector<byte>);
    void HandingDataTrigger(std::vector<byte>);
    void ConnectionErrorTrigger(int ErrorNumber);
    void (__closure *DataReadyForSendingTrigger)(std::vector<byte> Data);

    TComConnections(TComponent* _Owner);
    ~TComConnections();
};
//---------------------------------------------------------------------------
#endif
