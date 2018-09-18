#include <stdio.h>
#include <windows.h>


TCHAR * QueryRegedit(char * rootKey,  char * subKey, char * keyName){
    long lRet;
    HKEY hKey;
    TCHAR keyValue[64];

    //subKey = "PATH\\TO\\REGEDIT";
    
    if(!stricmp(rootKey, "HKCR"))
        lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, subKey, 0, KEY_QUERY_VALUE, &hKey);
    else if(!stricmp(rootKey, "HKCU"))
       lRet = RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_QUERY_VALUE, &hKey);
    else if(!stricmp(rootKey, "HKLM"))
       lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_QUERY_VALUE, &hKey);
    else if(!stricmp(rootKey, "HKU"))
       lRet = RegOpenKeyEx(HKEY_USERS, subKey, 0, KEY_QUERY_VALUE, &hKey);
    else if(!stricmp(rootKey, "HKCC"))
       lRet = RegOpenKeyEx(HKEY_CURRENT_CONFIG, subKey, 0, KEY_QUERY_VALUE, &hKey);
    else
        printf("Root Key error!!\n");


    if(lRet == ERROR_SUCCESS){
        DWORD dwSize = sizeof(keyValue);
        lRet = RegQueryValueEx(hKey, keyName, NULL, NULL, (LPBYTE)keyValue, &dwSize);   

        if(lRet != ERROR_SUCCESS)
            printf("keyValue Query error!\n");
        else
            return keyValue;
        
        RegCloseKey(hKey);
    }
    
    return NULL;
}


void main(){
    char rootKey[] = "HKLM";
    char subKey[] = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    char keyName[] = "ProcessorNameString";

    TCHAR * keyValue = malloc(sizeof(TCHAR) * 64);
    keyValue = QueryRegedit(rootKey, subKey, keyName);
    printf("keyValue: %s\n", keyValue);
}
