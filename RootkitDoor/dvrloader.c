#include <stdio.h>
#include <windows.h>
#include <process.h>

void usage(char * progName){
    printf("Usage:\n%s l serviceName path\\to\\*.sys\t#%s\n%s u serviceName\t\t#%s\n",
            progName, "load driver\n",
            progName, "unload driver\n");
}


int main(int argc, char* argv[]){
    if(argc < 2){
        usage(argv[0]);
        exit(0);
    }

    if(*argv[1] == 'l'){
        SC_HANDLE sh, rh;
        
        if(argc != 4){
            usage(argv[0]);
            exit(0);
        }

        sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if(!sh){
            printf("error: OpenSCManager failed\n");
            exit(1);
        }

        rh = CreateService(sh, argv[2], argv[2], SERVICE_ALL_ACCESS,
                SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, argv[3],
                NULL, NULL, NULL, NULL, NULL);

        if(!rh){
            if(GetLastError() == ERROR_SERVICE_EXISTS){
                printf("warning: Service is already loaded\n");
                exit(1);
            }else{
                printf("error: CreateService failed\n");
                exit(1);
            }
        }
    }

    else if(*argv[1] == 'u'){
        SC_HANDLE sh, rh;
        SERVICE_STATUS ss;

        if(argc != 3){
            usage(argv[0]);
            exit(0);
        }

        printf("Unloading rootkit driver\n");

        sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if(!sh){
            printf("error: OpenSCManager failed\n");
            exit(1);
        }

        rh = OpenService(sh, argv[2], SERVICE_ALL_ACCESS);
        if(!rh){
            printf("error: OpenService failed\n");
            CloseServiceHandle(sh);
            exit(1);
        }

        if(!ControlService(rh, SERVICE_CONTROL_STOP, &ss)){
        }

        if(!DeleteService(rh)){
            puts("error: couldn't delete service");
        }

        CloseServiceHandle(rh);
        CloseServiceHandle(sh);

    }
    else{
        usage(argv[0]);
        return 0;
    }
}
