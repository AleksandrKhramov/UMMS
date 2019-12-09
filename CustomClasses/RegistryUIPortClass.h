//---------------------------------------------------------------------------

#ifndef RegistryUIPortClassH
#define RegistryUIPortClassH
/*
	TRegistryUIPort - это класс для работы с реестром.
    Этот класс может читать и задавать значение порта UDP сервера, который запущен у службы UMMS_ConnectionService.

	Ссылки с описанием работы с реестром находятся в RegistryAbstractClass.h
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
