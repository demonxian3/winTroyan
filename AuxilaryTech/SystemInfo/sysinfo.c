#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#define MAX_BUFFER_LEN 512

//for memory info 
typedef struct _MEMORYSTATUSEX {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
typedef BOOL (CALLBACK* GLOBALMEMORYSTATUSEX)(MEMORYSTATUSEX* pMemStatus);


void getDirectoryPath(){
    TCHAR szPath[MAX_PATH];
    GetWindowsDirectory(szPath, MAX_PATH);
    printf("Windows dir: \t\t%s\n\r", szPath);
    GetSystemDirectory(szPath, MAX_PATH);
    printf("System dir: \t\t%s\n\r", szPath);
    GetCurrentDirectory(MAX_PATH, szPath);
    printf("Current dir: \t\t%s\n\r", szPath);
}

void getHostName(){
    TCHAR szName[MAX_BUFFER_LEN];
    DWORD dwNameLen = MAX_BUFFER_LEN;
    GetComputerName(szName, &dwNameLen);
    printf("Host name: \t\t%s\n\r", szName);
}

void getUserName(){
    TCHAR szName[MAX_BUFFER_LEN];
    DWORD dwNameLen = MAX_BUFFER_LEN;
    GetUserName(szName, &dwNameLen);
    printf("User name: \t\t%s\n\r", szName);
}

void getOSCode(){
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&os);
    switch(os.dwPlatformId){
        case VER_PLATFORM_WIN32_WINDOWS:
            printf("OS platform: \t\tWindows 9X\n\r");
        case VER_PLATFORM_WIN32_NT:
            printf("OS platform: \t\tWindows NT/2000\n\r");
    }

    GetVersionEx(&os);
    if(os.dwMajorVersion == 5 && os.dwMinorVersion == 0)
        printf("OS version: \t\t%s\n", "Windows 2000");
    else if(os.dwMajorVersion == 5 && os.dwMinorVersion == 1)
        printf("OS version: \t\t%s\n", "Windows XP");
    else if(os.dwMajorVersion == 5 && os.dwMinorVersion == 2)
        printf("OS version: \t\t%s\n", "Windows vista");
    else if(os.dwMajorVersion == 6 && os.dwMinorVersion == 0)
        printf("OS version: \t\t%s\n", "Windows 2003");
    else if(os.dwMajorVersion == 6 && os.dwMinorVersion == 1)
        printf("OS version: \t\t%s\n", "Windows 7");
    else if(os.dwMajorVersion == 6 && os.dwMinorVersion == 2)
        printf("OS version: \t\t%s\n", "Windows 10");

    return;
}

void getMemory(){
    MEMORYSTATUSEX mem;
    GLOBALMEMORYSTATUSEX pGlobalMemoryStatusEx;
    pGlobalMemoryStatusEx = (GLOBALMEMORYSTATUSEX)GetProcAddress(
            GetModuleHandle("Kernel32.dll"), "GlobalMemoryStatusEx");
    mem.dwLength = sizeof(mem);
    pGlobalMemoryStatusEx(&mem);
    printf("Mem tatol: \t\t%uMB\n\r", mem.ullTotalPhys/1024/1024);
}



void getDriver(){
    int i;
    UINT uType;
    TCHAR szDriver[MAX_BUFFER_LEN];
    TCHAR szFileSys[MAX_BUFFER_LEN];

    for(i=0; i<26; i++){
        wsprintf(szDriver, "%c:\\", 'A'+i);
        uType = GetDriveType(szDriver);

        switch(uType){
            case DRIVE_FIXED:
                GetVolumeInformation(szDriver, NULL, (DWORD)NULL, NULL, NULL, NULL, szFileSys, MAX_BUFFER_LEN);
                printf("Hardisk:  \t\t%s [%s]\n\r",szDriver, szFileSys);
                break;

            case DRIVE_CDROM:
                printf("CDROM: \t\t\t%s\n\r",szDriver);
                break;

            case DRIVE_REMOTE:
                GetVolumeInformation(szDriver, NULL, (DWORD)NULL, NULL, NULL, NULL, szFileSys, MAX_BUFFER_LEN);
                printf("REMOTE: \t\t%s [%s] \n\r",szDriver, szFileSys);
                break;
        }
    }

}


int main(void){
    printf("<<System information>>\n\r");
    
    getHostName();
    getUserName();
    getOSCode();
    getMemory();
    getDriver();
    getDirectoryPath();
    return 0;
}
