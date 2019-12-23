//---------------------------------------------------------------------------
#ifndef ComConnectionErrorsCPP
#define ComConnectionErrorsCPP
enum ComConnectionErrors
{
    ComConnectionOpenError,
    ComConnectionOpenInvalidHandleError,
    ComConnectionOpenGetStateError,
    ComConnectionOpenSetStateError,
    ComConnectionOpenSetTimeoutsError,
    ComConnectionDataPassError,
    ComConnectionDataPassExpectationError,
    ComConnectionReceivingDataHandleError,
    ComConnectionReceivingDataCountError,
    ComConnectionReceivingDataError,
    ComConnectionCloseError
};
#endif

