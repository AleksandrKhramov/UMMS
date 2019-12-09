//---------------------------------------------------------------------------
#include "Properties.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TRealValues::TRealValues()
{
	ValuesQuantity = 65;
    InitialAddress = 40001;

    for(int i = 0; i < ValuesQuantity; ++i)
        RealValues.push_back(new TAbstractValue(InitialAddress + i));
}
//---------------------------------------------------------------------------
//����������
TRealValues::~TRealValues()
{
	for(int i = 0; i < ValuesQuantity; ++i)
        delete RealValues[i];

    RealValues.clear();
    RealValues.~vector();
}
//---------------------------------------------------------------------------
void TRealValues::Set(std::vector<TAbstractValue *> Values)
{
	//
}
//---------------------------------------------------------------------------
void TRealValues::Update(std::vector<TAbstractValue *> Values)
{
	RealValues = Values;
}
//---------------------------------------------------------------------------
std::vector<TAbstractValue *> TRealValues::Read()
{
	return RealValues;
}
//---------------------------------------------------------------------------