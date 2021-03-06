//---------------------------------------------------------------------------

#ifndef MS3071InterfaceClassH
#define MS3071InterfaceClassH
/*
	TMS3071Interface -

*/
//---------------------------------------------------------------------------
//							  Built headers
#include <IdGlobal.hpp>
#include <vector>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "MS3071Class.h"
#include "ProtocolsClass.h"
//---------------------------------------------------------------------------
class TMS3071Interface
{
private:
public:
	std::vector<TMS3071 *> MS3071;
    TProtocols *Protocols;
    std::vector< std::vector<unsigned char> > SearchPatterns;
    std::vector< std::vector<unsigned char> > GetPatterns();

	TMS3071Interface();
	~TMS3071Interface();
};
//---------------------------------------------------------------------------

#endif
