//---------------------------------------------------------------------------
#include "MS3071InterfaceClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TMS3071Interface::TMS3071Interface()
{
	Protocols = new TProtocols;
    SearchPatterns = Protocols->GetPatterns();
}
//---------------------------------------------------------------------------
//����������
TMS3071Interface::~TMS3071Interface()
{

}
//---------------------------------------------------------------------------
std::vector< std::vector<unsigned char> > TMS3071Interface::GetPatterns()
{
	return SearchPatterns;
}