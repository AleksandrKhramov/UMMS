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
#include "ExternalDataHanding.cpp"
#include "ConnectionTypes.cpp"
//---------------------------------------------------------------------------
class TComConnections
{
private:
	TComponent* Owner;
    TTimer* SearchingTimer;
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
	void NotifyConnectionRemoved(int ComNumber);
    void NotifyDeviceConnected(TComConnection *ComConnection);
    void UpdateComLists();
    bool IsComPortExists(int ComNumber);
    bool IsListsIdentical(TStringList *, TStringList *);
    int  IndexOfComConnection(TComConnection *);
    void RemoveConnection(TComConnection *);
    void ExternalSend(int DataHanding, int ComNumber, std::vector<byte> Data = (std::vector<byte>)NULL);
    void __fastcall SearchingTimerOnTimer(TObject *Sender);
public:
	void ExternalConnectionsUpdate(bool Full = false);
    void HandingDataTrigger(std::vector<byte>);
    void (__closure *DataReadyForSendingTrigger)(std::vector<byte> Data);

    TComConnections(TComponent* _Owner);
    ~TComConnections();
};
//---------------------------------------------------------------------------
#endif
