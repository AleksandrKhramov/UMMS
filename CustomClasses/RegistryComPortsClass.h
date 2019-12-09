//---------------------------------------------------------------------------

#ifndef RegistryComPortsClassH
#define RegistryComPortsClassH

//---------------------------------------------------------------------------
//							  Built headers
#include <tuple>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "RegistryAbstractClass.h"
//---------------------------------------------------------------------------
class TRegistryComPorts : TRegistryAbstract
{
private:
	TStringList *ComNameList;
    TStringList *ComPortList;
public:
	TStringList *GetComNames();
    TStringList *GetComPorts();
    std::pair<TStringList*, TStringList*> GetComNamesAndPorts();

	TRegistryComPorts();
    ~TRegistryComPorts();
};
//---------------------------------------------------------------------------
#endif
