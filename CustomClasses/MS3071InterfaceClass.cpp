//---------------------------------------------------------------------------
#include "MS3071InterfaceClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TMS3071Interface::TMS3071Interface()
{
	Protocols = new TProtocols;
    SearchPatterns = Protocols->GetPatterns();
}
//---------------------------------------------------------------------------
//Деструктор
TMS3071Interface::~TMS3071Interface()
{

}
//---------------------------------------------------------------------------
std::vector< std::vector<unsigned char> > TMS3071Interface::GetPatterns()
{
	return SearchPatterns;
}
