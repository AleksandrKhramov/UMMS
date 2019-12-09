//---------------------------------------------------------------------------
#include "MS3071Class.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TMS3071::TMS3071()
{
	Name = new TName;
    Number = new TNumber;
	Class = new TAccuracyClass;
	ManufacturingData = new TManufacturingData;
	Password = new TPassword;
	Range = new TRange;
	Step = new TStep;
	EnteredValue = new TEnteredValue;
	ScreenMode = new TScreenMode;
    DecadesState = new TDecadesState;
    RealEnteredValue = new TRealEnteredValue;
    PercentageAccuracy = new TPercentageAccuracy;
    PreviousEnteredValue = new TPreviousEnteredValue;
    AbsoluteAccuracy = new TAbsoluteAccuracy;
    MaximalVoltage = new TMaximalVoltage;
    MaximalCurrent = new TMaximalCurrent;
	OutputMode = new TOutputMode;
	FirmwareVersion = new TFirmwareVersion;
	RealValues = new TRealValues;
}
//---------------------------------------------------------------------------
//Деструктор
TMS3071::~TMS3071()
{
	delete Name;
    delete Number;
	delete Class;
	delete ManufacturingData;
	delete Password;
	delete Range;
	delete Step;
	delete EnteredValue;
	delete ScreenMode;
    delete DecadesState;
    delete RealEnteredValue;
    delete PercentageAccuracy;
    delete PreviousEnteredValue;
    delete AbsoluteAccuracy;
    delete MaximalVoltage;
    delete MaximalCurrent;
	delete OutputMode;
	delete FirmwareVersion;
	delete RealValues;

}
//---------------------------------------------------------------------------
