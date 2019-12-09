//---------------------------------------------------------------------------

#ifndef RegistryUIPortClassH
#define RegistryUIPortClassH
/*
	TRegistryUIPort - ��� ����� ��� ������ � ��������.
    ���� ����� ����� ������ � �������� �������� ����� UDP �������, ������� ������� � ������ UMMS_ConnectionService.

	������ � ��������� ������ � �������� ��������� � RegistryAbstractClass.h
*/
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "RegistryAbstractClass.h"
//---------------------------------------------------------------------------
class TRegistryUIPort : TRegistryAbstract
{
private:
public:
	void UpdatePort(int PortNumber);
    int ReadPort();
	TRegistryUIPort();
    ~TRegistryUIPort();
};
//---------------------------------------------------------------------------
#endif
