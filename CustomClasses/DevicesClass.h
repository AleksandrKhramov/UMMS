
#ifndef DevicesClassH
#define DevicesClassH
/*
	TMS3071Interface -

*/
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "MS3071InterfaceClass.h"
#include "UDPInterconnectionUIClass.h"
//---------------------------------------------------------------------------
class TDevices
{
private:
public:
	TMS3071Interface *MS3071Interface;
    TUDPInterconnectionUI *UDPInterconnection;

	TDevices();
	~TDevices();
};
//---------------------------------------------------------------------------
#endif
