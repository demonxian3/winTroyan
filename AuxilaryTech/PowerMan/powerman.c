#include <windows.h>
#include <stdio.h>

//https://docs.microsoft.com/zh-cn/windows/desktop/Shutdown/how-to-shut-down-the-system

//adjust privileges
BOOL adjustProcPriv(){
    HANDLE hToken;
    TOKEN_PRIVILEGES tk;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)) 
        return FALSE;

    if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tk.Privileges[0].Luid))
        return FALSE;

    tk.PrivilegeCount = 1;
    tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if(!AdjustTokenPrivileges(hToken, FALSE, &tk, 0, (PTOKEN_PRIVILEGES)NULL, 0))
        return FALSE;

    return TRUE;
}

//notice: System Shutdown Reason Codes 0x00040000 => SHTDN_REASON_MAJOR_APPLICATION
BOOL logoff(){
    if(!adjustProcPriv() || !ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0x00040000))
        return TRUE;
    return FALSE;
}
BOOL poweroff(){
    if(!adjustProcPriv() || !ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0x00040000))
        return TRUE;
    return FALSE;
}
BOOL reboot(){
    if(!adjustProcPriv() || !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0x00040000))
        return TRUE;
    return FALSE;
}


int main(int argc, char *argv[]){
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(argc < 2){
        printf("usage:\npoweroff: this.exe -h/-s\nlogoff: this.exe -l/-q\nreboot: this.exe -r\n");    
        return FALSE;
    }

    //get os version, and not nt will quit
    if(GetVersionEx(&osvi) == 0)return FALSE;

    if(osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return FALSE;
    
    if(!stricmp(argv[1], "-h")) poweroff();
    if(!stricmp(argv[1], "-s")) poweroff();
    if(!stricmp(argv[1], "-r")) reboot();
    if(!stricmp(argv[1], "-q")) logoff();
    if(!stricmp(argv[1], "-l")) logoff();

    return TRUE;
}
