#include <stdio.h>
#include <windows.h>


DWORD * regQuery(char * rootKey,  char * subKey, char * keyName, TCHAR *keyValue, DWORD dwSize){
    DWORD lRet;
    HKEY hKey;

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
        lRet = RegQueryValueEx(hKey, keyName, NULL, NULL, (LPBYTE)keyValue, &dwSize);   

        if(lRet != ERROR_SUCCESS){
            printf("keyValue Query error!\n");
            memset(keyValue, 0, dwSize);
        }
        
        RegCloseKey(hKey);
        return 1;
    }
    
    return 0;
}

DWORD * regWrite(char * rootKey, char * subKey, char * keyName, char * keyValue, 
    DWORD dwSize, DWORD dwType){

    DWORD lRet,dwDisposition = REG_OPENED_EXISTING_KEY;
    HKEY hRootKey, hKey;
    

    if(!stricmp(rootKey, "HKCR"))
        hRootKey = HKEY_CLASSES_ROOT;
    else if(!stricmp(rootKey, "HKCU"))
        hRootKey = HKEY_CURRENT_USER;
    else if(!stricmp(rootKey, "HKLM"))
        hRootKey = HKEY_LOCAL_MACHINE;
    else if(!stricmp(rootKey, "HKU"))
        hRootKey = HKEY_USERS;
    else if(!stricmp(rootKey, "HKCC"))
        hRootKey = HKEY_CURRENT_CONFIG;
    else{
        printf("Root Key error!!\n");
        return 0;
    }
    
    lRet = RegCreateKeyEx(hRootKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, 
            KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

    if(lRet != ERROR_SUCCESS){
        printf("key open or create error, you can try run as administrator again\n");
        return 0;
    }

    lRet = RegSetValueEx(hKey, keyName, 0, dwType, (BYTE*)keyValue, dwSize);

    RegCloseKey(hKey);

    if(lRet == ERROR_SUCCESS){
        printf("write success\n");
        return 1;
    }else{
        printf("ret :%d\n", lRet);
        return 0;
    }
}

DWORD * regDelete(char * rootKey, char * subKey, char * keyName){
    DWORD lRet;
    HKEY hRootKey, hKey;

    if(!stricmp(rootKey, "HKCR"))
        hRootKey = HKEY_CLASSES_ROOT;
    else if(!stricmp(rootKey, "HKCU"))
        hRootKey = HKEY_CURRENT_USER;
    else if(!stricmp(rootKey, "HKLM"))
        hRootKey = HKEY_LOCAL_MACHINE;
    else if(!stricmp(rootKey, "HKU"))
        hRootKey = HKEY_USERS;
    else if(!stricmp(rootKey, "HKCC"))
        hRootKey = HKEY_CURRENT_CONFIG;
    else{
        printf("Root Key error!!\n");
        return 0;
    }
    
    lRet = RegOpenKeyEx(hRootKey, subKey, 0, KEY_ALL_ACCESS, &hKey);

    if(lRet == ERROR_SUCCESS){
        lRet = RegDeleteValue(hKey, keyName);

        if(lRet == ERROR_SUCCESS){
            printf("key delete success!!!\n");
        }else{
            printf("key delete failed\n");
        }
        
        RegCloseKey(hKey);
        return 1;
    }
    
    
}

DWORD * regDeleteItem(char * rootKey, char * subKey){
    DWORD lRet;
    HKEY hRootKey, hKey;

    if(!stricmp(rootKey, "HKCR"))
        hRootKey = HKEY_CLASSES_ROOT;
    else if(!stricmp(rootKey, "HKCU"))
        hRootKey = HKEY_CURRENT_USER;
    else if(!stricmp(rootKey, "HKLM"))
        hRootKey = HKEY_LOCAL_MACHINE;
    else if(!stricmp(rootKey, "HKU"))
        hRootKey = HKEY_USERS;
    else if(!stricmp(rootKey, "HKCC"))
        hRootKey = HKEY_CURRENT_CONFIG;
    else{
        printf("Root Key error!!\n");
        return 0;
    }
    
    lRet = RegOpenKeyEx(hRootKey, subKey, 0, KEY_ALL_ACCESS, &hKey);

    if(lRet == ERROR_SUCCESS){
        lRet = RegDeleteValue(hKey, subKey);

        if(lRet == ERROR_SUCCESS){
            printf("item delete success!!!\n");
        }else{
            printf("item delete failed\n");
        }
        
        RegCloseKey(hKey);
        return 1;
    }
}


void main(int argc, char *argv[]){
    DWORD dwType;
    char rootKey[] = "HKLM";
    char subKey1[] = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    char subKey2[] = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\3";
    char keyName[] = "ProcessorNameString";
    char keyName2[] = "myCreateInfo";
    char currentPath[MAX_PATH];
    TCHAR keyValue[128];
    GetModuleFileName(NULL, currentPath, MAX_PATH);

    //usage
    if(argc < 2){
        printf("usage example: \n");

        printf(" regm.exe get %s %s %s\n", rootKey, subKey1, keyName);
        //regQuery(rootKey, subKey1, keyName, keyValue, sizeof(keyValue));

        printf(" regm.exe set %s %s %s %s %s\n", rootKey, subKey2, keyName2, currentPath, "REG_SZ");
        //regWrite(rootKey, subKey2, keyName2, currentPath, strlen(currentPath), REG_SZ);

        printf(" regm.exe del %s %s %s", rootKey, subKey2, keyName2);
        //regDelete(rootKey, subKey2, keyName2);
    }else{
        if( argc >= 6 && !stricmp(argv[1], "set")){
            if(!stricmp(argv[6], "REG_SZ"))
                dwType = REG_SZ;
            else if(!stricmp(argv[6], "REG_BINARY"))
                dwType = REG_BINARY;
            else if(!stricmp(argv[6], "REG_DWORD"))
                dwType = REG_DWORD;
            else if(!stricmp(argv[6], "REG_NONE"))
                dwType = REG_NONE;
            //else if(!stricmp(argv[6], "REG_QWORD"))
            //    dwType = REG_QWORD;
            
            regWrite(argv[2], argv[3], argv[4], argv[5], strlen(argv[5]), dwType);
        
        }else if(argc == 5 && !stricmp(argv[1], "get")){
            regQuery(argv[2], argv[3], argv[4], keyValue, sizeof(keyValue));
            printf("%s\n", keyValue);
        }else if(argc == 5 && !stricmp(argv[1], "del")){
            regDelete(argv[2], argv[3], argv[4]);
        }else if(argc == 4 && !stricmp(argv[1], "del")){
            printf("item\n");
            regDeleteItem(argv[2], argv[3]);
        }else{
            printf("param error, please type `regm.exe` to look usage example\n");
        }

    }
    
}
