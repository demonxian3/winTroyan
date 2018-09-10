#include <windows.h>
#include <iostream.h>
#include <tlhelp32.h>
#include <stdio.h>

//
// PROCESSENTRY32{
//      DWORD dwSize;   #The size of the structure, it must set betfore calling Process32First
//      DWORD th32ProcessID;    #The process identifier.
//      DWORD cntThreads;       #The number of execution threads started by the process.
//      DWORD th32ParentProcessID; #(its parent process id
//      LONG pcPriClassBase;    #The base priority of any threads created by this process.
//      CHAR szExeFile[MAX_PATH] #The name of the executable file for the process. 
// }
DWORD getPIDByName(char * processName){
    DWORD Pid = -1;
    PROCESSENTRY32 lPrs;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //create system snapshot
    ZeroMemory(&lPrs, sizeof(lPrs));
    lPrs.dwSize = sizeof(lPrs);
    Process32First(hSnap, &lPrs);



    //get first process's info in system snapshot
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
            printf("th32ProcessID :%lu\n", lPrs.th32ProcessID);
            printf("szExeFile :%s\n", lPrs.szExeFile);
            printf("th32ParentProcessID :%lu\n", lPrs.th32ParentProcessID);
            printf("cntThreads: %lu\n", lPrs.cntThreads);
            printf("pcPriClassBase: %lu\n", lPrs.pcPriClassBase);
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
    printf("process.exe example:\n");
    printf("===========================================\n");
    printf("    list process:  process.exe -l \n");
    printf("    special imgName: process.exe -n <process name>\n");
}

void main(int argc, char * argv[]){
    DWORD pid;
    char processName[200];

    if(argc < 2){
        showUsageHelp();
        return ;
    }

    if(!stricmp(argv[1], "-l")){
        listAllProcessInfo();
        return ;
    }

    else if(!stricmp(argv[1], "-n")){
        getPIDByName(argv[2]);
        return;
    }
    
}
