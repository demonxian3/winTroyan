/*
 * func: pause update continue stop start the services like sc.exe
 */

#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <windows.h>

int simpleSetConfiguration(SC_HANDLE schService, DWORD startType){
    DWORD dwServiceType = SERVICE_NO_CHANGE;
    DWORD dwStartType = SERVICE_NO_CHANGE;
    DWORD dwErrorControl = SERVICE_NO_CHANGE;
    LPCTSTR lpBinaryPathName = NULL;
    LPCTSTR lpLoadOrderGroup = NULL;
    LPDWORD lpdwTagId = NULL;
    LPCTSTR lpDependencies = NULL;
    LPCTSTR lpServiceStartName = NULL;
    LPCTSTR lpPassword = NULL;
    LPCTSTR lpDisplayName = NULL;

    if(startType)
        dwStartType = startType;

    return ChangeServiceConfig(
            schService,         //hanle
            dwServiceType,      //service type
            dwStartType,        //service start type
            dwErrorControl,     //error control
            lpBinaryPathName,   //binary path
            lpLoadOrderGroup,   //load order group
            lpdwTagId,          //tag ID
            lpDependencies,     //dependencies
            lpServiceStartName, //account name
            lpPassword,         //password
            lpDisplayName       //display name
    );
}


int simpleUpdateDescription(SC_HANDLE schService, LPCTSTR szDesc){
    SERVICE_DESCRIPTION sd;
    sd.lpDescription = szDesc;
    return ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd);
}

int simpleQueryServiceConfiguration(SC_HANDLE schService, char * serviceName){
    int ret;
    LPQUERY_SERVICE_CONFIG lpsc;
    LPSERVICE_DESCRIPTION lpsd;
    DWORD dwBytesNeeded, cbBufSize, dwError;

    ret = QueryServiceConfig(schService, NULL, 0, &dwBytesNeeded); //first query to get length of lpsc struct
    if(!ret){                           //normal, no special point because of struct's length
        dwError = GetLastError();
        if(ERROR_INSUFFICIENT_BUFFER == dwError){
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBufSize); //alloc space to lpsd?
        }else
            return 0; //error
    }

    ret = QueryServiceConfig(schService, lpsc, cbBufSize, &dwBytesNeeded); //second query to get real info
    if(!ret) return 0; //error

    ret = QueryServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded);
    if(!ret) {                          //normal, no specail point and len beacuse of getting struct's len;
        dwError = GetLastError();
        if(ERROR_INSUFFICIENT_BUFFER == dwError){
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize); //alloc space to lpsd?
        }else
            return 0; //error
    }

    ret = QueryServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpsd, cbBufSize, &dwBytesNeeded);
    if(!ret) return 0; //error;

    //print information
    printf(" %s configuration: \n", serviceName);
    printf("    Type: 0x%x\n", lpsc->dwServiceType);
    printf("    Start Type: 0x%x\n", lpsc->dwStartType);
    printf("    Error Control: 0x%x\n", lpsc->dwErrorControl);
    printf("    Binary Path: %s\n", lpsc->lpBinaryPathName);
    printf("    Account: %s\n", lpsc->lpServiceStartName);

    if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
        printf("  Description: %s\n", lpsd->lpDescription);
    if (lpsc->lpLoadOrderGroup != NULL && lstrcmp(lpsc->lpLoadOrderGroup, TEXT("")) != 0)
        printf("  Load order group: %s\n", lpsc->lpLoadOrderGroup);
    if (lpsc->dwTagId != 0)
        printf("  Tag ID: %d\n", lpsc->dwTagId);
    if (lpsc->lpDependencies != NULL && lstrcmp(lpsc->lpDependencies, TEXT("")) != 0)
        printf("  Dependencies: %s\n", lpsc->lpDependencies);
 
    return 1; //ok
}

void showUsageHelp(){
    printf("scm.exe usage:\n");
    printf("================================\n");
    printf("  Author: Demon\n");
    printf("================================\n");
    printf("  Example: \n");
    printf("    Control: \n");
    printf("      open service: scm.exe start serviceName: \n");
    printf("      stop service: scm.exe stop serviceName: \n");
    printf("      puase service: scm.exe pause serviceName: \n");
    printf("      continue service: scm.exe continue serviceName: \n\n");
    printf("    Config: \n");
    printf("      start type demand: scm.exe demand serviceName\n");
    printf("      start type auto: scm.exe auto serviceName\n");
    printf("      start type disabled: scm.exe disabled serviceName\n\n");
    printf("      set description: scm.exe desc serviceName \"This is description example\"\n\n");
    printf("    Query: \n");
    printf("      query status: scm.exe status serviceName\n");
    printf("      query config: scm.exe config serviceName\n\n");
    printf("================================\n");
    return;
}

void showServiceTypeInfo(serviceType){
    if(!serviceType) return;
    switch(serviceType){
        case SERVICE_FILE_SYSTEM_DRIVER:
            printf(" (The service is a file system driver.) \n");
            break;
        case SERVICE_KERNEL_DRIVER:
            printf(" (The service is a device driver.) \n");
            break;
        case SERVICE_WIN32_OWN_PROCESS:
            printf(" (The service runs in its own process. ) \n");
            break;
        case SERVICE_WIN32_SHARE_PROCESS:
            printf(" (The service shares a process with other services. ) \n");
            break;
    }
}

void showServiceCurrentState(serviceState){
    if(!serviceState) return;
    switch(serviceState){
        case SERVICE_CONTINUE_PENDING:
            printf(" (The service continue is pending. ) \n");
            break;
        case SERVICE_PAUSE_PENDING:
            printf(" (The service pause is pending. ) \n");
            break;
        case SERVICE_PAUSED:
            printf(" (The service is paused. ) \n");
            break;
        case SERVICE_RUNNING:
            printf(" (The service is running. ) \n");
            break;
        case SERVICE_START_PENDING:
            printf(" (The service is starting. ) \n");
            break;
        case SERVICE_STOP_PENDING:
            printf(" (The service is stopping. ) \n");
            break;
        case SERVICE_STOPPED:
            printf(" (The service is not running. ) \n");
            break;
    }
}

int main(int argc, char *argv[]){
    if(argc >= 3){
        int ret = 0;
        SC_HANDLE schSCManager;     //for: OpenSCManager
        SC_HANDLE schService;       //for: OpenService
        SERVICE_STATUS status;      //for: QueryServiceStatus
        char serviceName[100];      

        strcpy(serviceName, argv[2]);   //get serviceName from cmd line param
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //open scm with all privileges
        if(NULL == schSCManager){
            printf("OpenSCManager failed: %d\n", GetLastError());
            return 1;
        }

        //ACR => FUNC
        //https://docs.microsoft.com/zh-cn/windows/desktop/Services/service-security-and-access-rights
        //SERVICE_ALL_ACCESS => Include all blow;
        //SERVICE_CHANGE_CONFIG => ChangeServiceConfig,  ChangeServiceConfig2
        //SERVICE_ENUMERATE_DEPENDENTS => EnumDependentServices 
        //SERVICE_INTERROGATE, SERVICE_STOP, SERVICE_USER_DEFINED_CONTROL => ControlService
        //SERVICE_PAUSE_CONTINUE => ControlService
        //SERVICE_QUERY_CONFIG => QueryServiceConfig, QueryServiceConfig2
        //SERVICE_START => StartService 
        //SERVICE_QUERY_STATUS =>  QueryServiceStatus,  QueryServiceStatusEx, NotifyServiceStatusChange 
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS); //open service with all privileges by scm
        if(NULL == schService){
            printf("OpenService failed: %d\n", GetLastError());
            CloseServiceHandle(schSCManager);
            return 1;
        }

        if(!stricmp(argv[1], "demand"))
            ret = simpleSetConfiguration(schService, SERVICE_DEMAND_START);
        else if(!stricmp(argv[1], "auto"))
            ret = simpleSetConfiguration(schService, SERVICE_AUTO_START);
        else if(!stricmp(argv[1], "disabled"))
            ret = simpleSetConfiguration(schService, SERVICE_DISABLED);
        else if(!stricmp(argv[1], "desc")){
            LPCTSTR szDesc = TEXT(argv[3]);
            ret = simpleUpdateDescription(schService, szDesc);
        }else if(!stricmp(argv[1], "status")){
            ret = QueryServiceStatus(schService, &status);
            if(ret){
                printf("Type: %lu", status.dwServiceType);
                showServiceTypeInfo(status.dwServiceType);
                printf("State: %lu", status.dwCurrentState);
                showServiceCurrentState(status.dwCurrentState);
                //printf("Controls accpeted: %lu\n", status.dwControlsAccepted);
                //printf("Win32 exit code: %lu\n", status.dwWin32ExitCode);
                //printf("Specific exit code: %lu\n", status.dwServiceSpecificExitCode);
                //printf("Check point: %lu\n", status.dwCheckPoint);
                //printf("Wait hint: %lu\n", status.dwWaitHint);
            }
        }else if(!stricmp(argv[1], "stop")){
            ret = ControlService(schService, SERVICE_CONTROL_STOP, &status);
        }else if(!stricmp(argv[1], "pause")){
            ret = ControlService(schService, SERVICE_CONTROL_PAUSE, &status);
        }else if(!stricmp(argv[1], "continue")){
            ret = ControlService(schService, SERVICE_CONTROL_CONTINUE, &status);
        }else if(!stricmp(argv[1], "start")){
            ret = StartServiceA(schService, NULL, NULL);
        }else if(!stricmp(argv[1], "config")){
            ret = simpleQueryServiceConfiguration(schService, serviceName);
        }else{
            showUsageHelp();
        }

        if(!ret) 
            printf("service operate failed: %d\n", GetLastError());
        else
            printf("service operate success!!\n");

        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);

    }else{
        showUsageHelp();
    }
    return 0;
}

