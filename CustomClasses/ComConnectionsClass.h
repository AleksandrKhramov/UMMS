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
#include "IteratorByPatternsClass.h"
#include "ComConnectionErrors.cpp"
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
    TIteratorByPatterns *IteratorByPatterns;
    TStringList *ComNameList;
    TStringList *ComPortList;

    void SearchDevices();
	void DataReadyTrigger(TComConnection *, std::vector<byte>);
    void ConnectionErrorTrigger(TComConnection *, int ErrorNumber);
    bool IsAllActiveConnectionsExists();
    bool IsComListUpdated();
    void UpdateActiveConnections();
    void RemoveNonexistentConnections();
    void AddNewConnections();
	void NotifyDeviceDeleted(int ComNumber);
    void UpdateComLists();
    bool IsComPortExists(int ComNumber);
    bool IsListsIdentical(TStringList *, TStringList *);
    int IndexOfComConnection(TComConnection *);
public:
    void HandingDataTrigger(std::vector<byte>);
    void (__closure *DataReadyForSendingTrigger)(std::vector<byte> Data);

    TComConnections(TComponent* _Owner);
    ~TComConnections();
};
//---------------------------------------------------------------------------
#endif
