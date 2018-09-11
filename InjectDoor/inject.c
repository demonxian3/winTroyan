#include <windows.h>
#include <iostream.h>
#include <tlhelp32.h>
#include <stdio.h>

int EnableDebugPriv(){
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    //Open process token 
    BOOL ret = OpenProcessToken(
            GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken
    );

    if(!ret){
        printf("cannot get the token\n");return 1;
    }else{
        printf("token success\n");
    }

    //Gain process local ID
    ret = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    if(!ret){
        printf("cannot get the luid\n"); return 1;
    }else{
        printf("luid success\n");
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid = luid;

    //adjust privilege
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

    if(GetLastError() != ERROR_SUCCESS){
        printf("debug mode open success!!!\n");
    }else{
        printf("debug mode open failed\n");
    }
    return 0;
}

BOOL InjectDLL(const char *DllFullPath, const DWORD dwRemoteProcessId){
    HANDLE hRemoteProcess, hRemoteThread;
    char * pszLibFileRemote;
    PTHREAD_START_ROUTINE pfnStartAddr;

    //open debug mode to get high privilege 
    //It is not necessary to turn on debug mode.
    EnableDebugPriv();

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

DWORD getPIDByProcessName(char * processName){
    DWORD Pid = -1;
    PROCESSENTRY32 lPrs;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    ZeroMemory(&lPrs, sizeof(lPrs));
    lPrs.dwSize = sizeof(lPrs);
    Process32First(hSnap, &lPrs);

    if(strstr(processName, lPrs.szExeFile)){ 
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

        if(strstr(processName, lPrs.szExeFile)){
            Pid = lPrs.th32ProcessID;
            break;
        }
    }

    return Pid;
}

void listAllProcessInfo(){
    PROCESSENTRY32 lPrs;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    ZeroMemory(&lPrs, sizeof(lPrs));
    lPrs.dwSize = sizeof(lPrs);
    Process32First(hSnap, &lPrs);

    printf("pid\t\tppid\t\tname\n");
    printf("-------------------------------------------\n");

    while(1){
        printf("%d\t\t%d\t\t%s\n", lPrs.th32ProcessID, lPrs.th32ParentProcessID, lPrs.szExeFile);
        if(!Process32Next(hSnap, &lPrs)) break;
    }    
}

void showUsageHelp(){
    printf("inject.exe example:\n");
    printf("============================================================\n");
    printf("    list process         :  inject.exe -l \n");
    printf("    special process Name :  inject.exe -n <process name> <dll name> \n");
    printf("    special process ID   :  inject.exe -p <PID> <dll name> \n");
    printf("    make sure dll file is exists below current directory! \n ");
}

DWORD string2dword(char * str){
    DWORD i = 0, len = strlen(str), sum = 0;

    for(;i<len; i++){
        DWORD tmp = *(str+i) - 48;

        switch(len-i-1){
            case 0:
                sum += tmp;break;
            case 1:
                sum += tmp*10; break;
            case 2:
                sum += tmp*100; break;
            case 3:
                sum += tmp*1000; break;
            case 4:
                sum += tmp*10000; break;
            default:
                printf("invalid pid!!\n");
                return -1;
        }
    }

    return sum;
}

void main(int argc, char *argv[]){

    DWORD pid;
    char DllPath[MAX_PATH];

    if(argc < 2){
        showUsageHelp();
        return;
    }

    if(!stricmp(argv[1], "-l")){
        listAllProcessInfo();
        return;
    }

    if(!argv[3]){
        printf("please special DLL filename.dll first\n");
        return;
    }

    GetCurrentDirectory(MAX_PATH, DllPath);
    strcat(DllPath, "\\");
    strcat(DllPath, argv[3]);

    if(!stricmp(argv[1], "-n")){
        pid = getPIDByProcessName(argv[2]);
        printf("pid: %d\n", pid);
    }

    else if(!stricmp(argv[1], "-p")){
        pid = string2dword(argv[2]);
    }
    
    InjectDLL(DllPath, pid);
}
