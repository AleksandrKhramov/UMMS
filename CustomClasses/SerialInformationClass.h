//---------------------------------------------------------------------------
#ifndef SerialInformationClassH
#define SerialInformationClassH
//---------------------------------------------------------------------------
//							  Built headers
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>
#include <vector>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TSerialInformation
{
	struct TDetailedSerialInformation
    {
        std::string a;
    };

private:
	GUID *guidDev;
    HDEVINFO hDevInfo;
    SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData;
    std::vector<TDetailedSerialInformation *> DetailedSerialInformation;

    void RefreshDeviceList();
public:

    const std::vector<TDetailedSerialInformation *>& GetSerialInformation();


    TSerialInformation();
    ~TSerialInformation();
};
#endif
