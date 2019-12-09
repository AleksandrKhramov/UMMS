//---------------------------------------------------------------------------

#ifndef ProtocolsClassH
#define ProtocolsClassH
/*
	TProtocols -

*/
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ModbusProtocolClass.h"
//---------------------------------------------------------------------------
class TProtocols
{
private:
public:
	TModbusProtocol *ModbusProtocol;
    std::vector< std::vector<unsigned char> > GetPatterns();

	TProtocols();
	~TProtocols();
};
//---------------------------------------------------------------------------
#endif
