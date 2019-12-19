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
    ComConnectionReceivingDataError,
    ComConnectionCloseError
};
#endif

