//---------------------------------------------------------------------------
#include "AbstractValueClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TAbstractValue::TAbstractValue(unsigned int _RequestAddress)
{
	RequestAddress = _RequestAddress;
	AbstractValue = "";
}
//---------------------------------------------------------------------------
//����������
TAbstractValue::~TAbstractValue()
{

}
//---------------------------------------------------------------------------
String TAbstractValue::Read()
{
	return AbstractValue;
}
//---------------------------------------------------------------------------
void TAbstractValue::Update(String Value)
{
    AbstractValue = Value;
}
