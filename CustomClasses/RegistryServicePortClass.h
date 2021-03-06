//---------------------------------------------------------------------------

#ifndef RegistryServicePortClassH
#define RegistryServicePortClassH

//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "RegistryAbstractClass.h"
//---------------------------------------------------------------------------
class TRegistryServicePort : TRegistryAbstract
{
private:
public:
	void UpdatePort(int PortNumber);
    int ReadPort();

	TRegistryServicePort();
    ~TRegistryServicePort();
};
//---------------------------------------------------------------------------
#endif
