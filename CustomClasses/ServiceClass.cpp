//---------------------------------------------------------------------------
#include "ServiceClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TService::TService()
{

}
//---------------------------------------------------------------------------
//Деструктор
TService::~TService()
{

}
//---------------------------------------------------------------------------
int TService::Install(const char *FullPath, const char *ServiceName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        return ServiceControlManagerOpenError;
    }


    // Create the service
    schService = CreateService(
        schSCManager,              // SCM database
        ServiceName,               // name of service
        ServiceName,               // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START,        // start type
        SERVICE_ERROR_NORMAL,      // error control type
        FullPath,                  // path to service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        NULL,                      // no dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password


    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return CreateServiceError;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return -1;

	//WinExec("D:/Workspace/MAIN/UMMS/ConnectionService/Output/UMMS_ConnectionService.exe /INSTALL", SW_SHOW);
}
//---------------------------------------------------------------------------
int TService::Unistall(const char *FullPath, const char *ServiceName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        return ServiceControlManagerOpenError;
    }


    // Get a handle to the service.
    schService = OpenService(
        schSCManager,       // SCM database
        ServiceName,        // name of service
        DELETE);            // need delete access

    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return ServiceOpenError;
    }


    // Delete the service.
    if(!DeleteService(schService))
    {
    	CloseServiceHandle(schService);
    	CloseServiceHandle(schSCManager);
        return DeleteServiceError;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return -1;

	//WinExec("D:/Workspace/MAIN/UMMS/ConnectionService/Output/UMMS_ConnectionService.exe /UNINSTALL", SW_SHOW);
}
//---------------------------------------------------------------------------
int TService::StartInstalledService(const char *ServiceName)
{
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    SC_HANDLE schSCManager;
	SC_HANDLE schService;


    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // servicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
        return ServiceControlManagerOpenError;


    // Get a handle to the service.
    schService = OpenService(
        schSCManager,         // SCM database
        ServiceName,            // name of service
        SERVICE_ALL_ACCESS);  // full access

    if (schService == NULL)
    {
        return ServiceOpenError;
    }


    // Check the status in case the service is not stopped.
    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // size needed if buffer is too small
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return CheckStatusError;
    }


    // Check if the service is already running. It would be possible
    // to stop the service here, but for simplicity this example just returns.
    if(ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return AlreadyRunningError;
    }


    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;


    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is
        // one-tenth of the wait hint but not less than 1 second
        // and not more than 10 seconds.
        dwWaitTime = ssStatus.dwWaitHint/10;

        if(dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);


        // Check the status until the service is no longer stop pending.
        if (!QueryServiceStatusEx(
                schService,                     // handle to service
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE) &ssStatus,             // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded))              // size needed if buffer is too small
        {
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return SecondCheckStatusError;
        }

        if(ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
            {
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return TimeoutWaitingError;
            }
        }
    }


    // Attempt to start the service.
    if (!StartService(
            schService,  // handle to service
            0,           // number of arguments
            NULL))      // no arguments
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return StartError;
    }


    // Check the status until the service is no longer start pending.
    if (!QueryServiceStatusEx(
            schService,                     // handle to service
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return ThirdCheckStatusError;
    }


    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is
        // one-tenth the wait hint, but no less than 1 second and no
        // more than 10 seconds.
        dwWaitTime = ssStatus.dwWaitHint/10;

        if(dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);


        // Check the status again.
        if (!QueryServiceStatusEx(
            schService,             // handle to service
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
        {
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint.
                break;
            }
        }
    }


    // Determine whether the service is running.
    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }
    else
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return FailedStartError;
    }
}
//---------------------------------------------------------------------------
