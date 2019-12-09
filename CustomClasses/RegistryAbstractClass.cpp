//---------------------------------------------------------------------------
#include "RegistryAbstractClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//�����������
TRegistryAbstract::TRegistryAbstract(const char *Key)
{
	Registry = new TRegistry();
    Registry->RootKey = HKEY_LOCAL_MACHINE;
    Registry->OpenKey(Key, true);				//true ������ ���������� ��������, ��� ���� ������ ����� �� ����������, �� �� ����� ������

}
//---------------------------------------------------------------------------
//����������
TRegistryAbstract::~TRegistryAbstract()
{
    Registry->CloseKey();
    Registry->Free();
}
//---------------------------------------------------------------------------

