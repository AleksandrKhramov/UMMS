//---------------------------------------------------------------------------
#include "SerialInformationClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TSerialInformation::TSerialInformation()
{
    guidDev = new GUID();
    guidDev->Data1 = 0x86e0d1e0L;
    guidDev->Data2 = 0x8089;
    guidDev->Data3 = 0x11d0;
    guidDev->Data4[0] = 0x9c;
    guidDev->Data4[1] = 0xe4;
    guidDev->Data4[2] = 0x08;
    guidDev->Data4[3] = 0x00;
    guidDev->Data4[4] = 0x3e;
    guidDev->Data4[5] = 0x30;
    guidDev->Data4[6] = 0x1f;
    guidDev->Data4[7] = 0x73;

    //HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	//SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;


}
//---------------------------------------------------------------------------
//Деструктор
TSerialInformation::~TSerialInformation()
{
	delete guidDev;
}
//---------------------------------------------------------------------------
