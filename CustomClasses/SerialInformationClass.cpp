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

    hDevInfo = INVALID_HANDLE_VALUE;
	pDetData = NULL;
}
//---------------------------------------------------------------------------
//Деструктор
TSerialInformation::~TSerialInformation()
{
	delete guidDev;
}
//---------------------------------------------------------------------------
void TSerialInformation::RefreshDeviceList()
{
    try
	{
		hDevInfo = SetupDiGetClassDevs( guidDev, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

		if(hDevInfo == INVALID_HANDLE_VALUE)
		{

		}

		// Enumerate the serial ports
		bool bOk = true;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    	for (DWORD ii = 0; bOk; ii++)
        {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, ii, &ifcData);
			if (bOk)
            {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifcData, pDetData, dwDetDataSize, NULL, &devdata);

				if (bOk)
                {

					// Got a path to the device. Try to get some more info.

				}
				else
                {
                	GetLastError();
				}
			}
			else
            {

				DWORD err = GetLastError();
				if(err != ERROR_NO_MORE_ITEMS)
                {
                 	err;
				}
			}
		}
    }
    catch(...)
    {
    	//Exception handing ...
    }
}
