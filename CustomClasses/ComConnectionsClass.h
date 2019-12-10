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
	TComponent* Owner;
	std::vector<TComConnection *> ComConnections;
    std::vector< std::vector<byte> > Patterns;
    TStringList *ComNameList;
    TStringList *ComPortList;

    void SearchDevices();
    void AddPattern(std::vector<byte>);
	void DataReadyTrigger(TComConnection *, std::vector<byte>);
    void ConnectionErrorTrigger(int ErrorNumber);
    bool IsAllActiveConnectionsExists();
    bool IsComListUpdated();
    void UpdateActiveConnections();
    void RemoveNonexistentConnections();
    void AddNewConnections();
	void NotifyDeviceDeleted(int ComNumber);
    void UpdateComLists();
    void IsComPortExists(int ComNumber);
public:
    void HandingDataTrigger(std::vector<byte>);
    void (__closure *DataReadyForSendingTrigger)(std::vector<byte> Data);

    TComConnections(TComponent* _Owner);
    ~TComConnections();
};
//---------------------------------------------------------------------------
#endif
