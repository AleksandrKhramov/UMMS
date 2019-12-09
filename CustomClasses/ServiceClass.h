
#ifndef ServiceClassH
#define ServiceClassH

/*
	TService - ��� ����� ��� ������ �� ������� Windows.
    ���� ����� ����� �������������, �������, ���������, ...

    ��� �������:
    https://docs.microsoft.com/en-us/windows/win32/services/using-services
*/
//---------------------------------------------------------------------------
//							  Built headers
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <aclapi.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers

//---------------------------------------------------------------------------
class TService
{
	enum ServiceErrors
    {
    	ServiceControlManagerOpenError,
        ServiceOpenError,
        CheckStatusError,
        AlreadyRunningError,
        SecondCheckStatusError,
        TimeoutWaitingError,
        StartError,
        ThirdCheckStatusError,
        FourthCheckStatusError,
        FailedStartError,
        CreateServiceError,
        DeleteServiceError
    };

private:
public:
	int Install(const char *FullPath, const char *ServiceName);
    int Unistall(const char *FullPath, const char *ServiceName);
    int StartInstalledService(const char *ServiceName);

    TService();
    ~TService();
};
//---------------------------------------------------------------------------
#endif

