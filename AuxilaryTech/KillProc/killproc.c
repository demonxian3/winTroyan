#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "kernel32.lib")
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <tlhelp32.h>

void EnableDebugPriv(void){
    HANDLE hToken;
    TOKEN_PRIVILEGES tk;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)){
        printf("OpenProcessToken is Error\n");
        return ;
    }

    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tk.Privileges[0].Luid)){
        printf("LookupPrivilegeValue is Error\n");
        return ;
    }

    tk.PrivilegeCount = 1;
    tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if(!AdjustTokenPrivileges(hToken, FALSE, &tk, 0, (PTOKEN_PRIVILEGES)NULL, 0)){
        printf("AdjuestTokenPrivileges is Error\n");
        return  ;
    }

    printf("debug ok!!\n");
    CloseHandle(hToken);
    return;
}

void ProcKiller(int pid){
    DWORD ret;
    HANDLE hProc = NULL;

    hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if(hProc == NULL){
        printf("OpenProcess failed:%d\n", GetLastError());
        return ;
    }

    ret = TerminateProcess(hProc, 0);
    if(!ret){
        printf("TerminateProcess failed:%d\n", GetLastError());
        return ;
    }
}

void listAllCurProc(void){
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 procInfo;
    BOOL bProc;

    if(hSnapShot != INVALID_HANDLE_VALUE){
        ZeroMemory(&procInfo, sizeof(procInfo));
        procInfo.dwSize = sizeof(procInfo);

        bProc = Process32First(hSnapShot, &procInfo);
        do{
            printf("pid: %d\t%s\n", procInfo.th32ProcessID, procInfo.szExeFile);
        }while(bProc = Process32Next(hSnapShot, &procInfo));
        
    }

    CloseHandle(hSnapShot);
}

int main(int argc, char *argv[]){

    if(argc < 2){
        listAllCurProc();
        printf("\n\n*** kill the process by pid ***\n----------------------------------------\nusage: this.exe -k <pid>\n");
    }else{
        EnableDebugPriv();   
        ProcKiller(atoi(argv[2]));
    }
    return 0;
}
