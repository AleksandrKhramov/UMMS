
#ifndef RegistryAbstractClassH
#define RegistryAbstractClassH
/*
	TRegistryAbstract - ��� ����� ��� ������ � ��������.
    ���� ����� ������������� TRegistry � �������� � ������������ ������ � �������� ������ HKEY_LOCAL_MACHINE.

	������� � �������� �������� ������ � ��������:
    http://www.cyberforum.ru/cpp-builder/thread159991.html
*/
//---------------------------------------------------------------------------
//							  Built headers
#include "registry.hpp"
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TRegistryAbstract
{
private:
public:
	TRegistry* Registry;
	TRegistryAbstract(const char *Key);
	/* TCustomRegistry.Destroy */    virtual ~TRegistryAbstract();
};
//---------------------------------------------------------------------------
#endif
