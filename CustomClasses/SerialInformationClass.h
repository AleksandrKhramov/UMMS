//---------------------------------------------------------------------------

#ifndef SerialInformationClassH
#define SerialInformationClassH

//---------------------------------------------------------------------------
//							  Built headers
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TSerialInformation
{
	GUID *guidDev;

    TSerialInformation();
    ~TSerialInformation();
};
#endif
