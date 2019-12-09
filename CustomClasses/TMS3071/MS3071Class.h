
#ifndef MS3071ClassH
#define MS3071ClassH
/*
	TMS3071 -

*/
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "Properties.h"
//---------------------------------------------------------------------------
class TMS3071
{
private:
public:
	unsigned char DeviceAddress;
    int DeviceIndex;

	TName *Name;
    TNumber *Number;
	TAccuracyClass *Class;
	TManufacturingData *ManufacturingData;
	TPassword *Password; 
	TRange *Range;
	TStep *Step;
	TEnteredValue *EnteredValue;
	TScreenMode *ScreenMode;
    TDecadesState *DecadesState;
    TRealEnteredValue *RealEnteredValue;
    TPercentageAccuracy *PercentageAccuracy;
    TPreviousEnteredValue *PreviousEnteredValue;
    TAbsoluteAccuracy *AbsoluteAccuracy;
    TMaximalVoltage *MaximalVoltage;
    TMaximalCurrent *MaximalCurrent;
	TOutputMode *OutputMode;
	TFirmwareVersion *FirmwareVersion;
	TRealValues *RealValues;

	TMS3071();
	~TMS3071();
};
//---------------------------------------------------------------------------
#endif
