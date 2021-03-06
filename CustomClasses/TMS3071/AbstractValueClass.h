//---------------------------------------------------------------------------
#ifndef AbstractValueClassH
#define AbstractValueClassH
/*
	TAbstractValue - ��� �����, �� �������� ����������� ��������� �������� �������,
    � ������� ���� ������� ������/������ ������ �������� ���� String

*/
//---------------------------------------------------------------------------
//							  Built headers
#include "System.hpp"
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TAbstractValue
{
private:
	String AbstractValue;
    unsigned int RequestAddress;
public:
    String Read();
    void Update(String Value);

	TAbstractValue(unsigned int _RequestAddress);
	virtual ~TAbstractValue();
};
//---------------------------------------------------------------------------
#endif
