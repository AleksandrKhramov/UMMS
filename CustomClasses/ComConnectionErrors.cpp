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
    ComConnectionReceivingDataError,
    ComConnectionCloseError
};
#endif

