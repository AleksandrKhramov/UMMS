//---------------------------------------------------------------------------
#ifndef ModbusProtocolClassH
#define ModbusProtocolClassH
/*
	TModbusProtocol -

*/
//---------------------------------------------------------------------------
//							  Built headers
#include <vector>
#include "System.Classes.hpp"
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TModbusProtocol
{
private:
public:
	std::vector< std::vector<unsigned char> > GetPatterns();
    unsigned short MakeCRC16(std::vector<unsigned char> Buffer);
    std::vector<unsigned char> MakeMessage(unsigned char , unsigned char , unsigned short);
    std::vector<unsigned char> MakeMessage(unsigned char , unsigned char , unsigned short , std::vector<unsigned char>);

	TModbusProtocol();
	~TModbusProtocol();
};
//---------------------------------------------------------------------------
#endif
