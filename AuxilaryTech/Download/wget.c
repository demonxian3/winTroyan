#pragma comment(lib, "Urlmon")
#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[]){

    char dwnDiretory[MAX_PATH];

    HRESULT urldown;
    
    if(argc < 3){
        printf("usage: this.exe <url> <filename> [download directory] \n");
        return 0;
    }

    if(argc == 4){
        strcpy(dwnDiretory, argv[3]);
    }else{
        GetCurrentDirectory(MAX_PATH, dwnDiretory);
    }

    strcat(dwnDiretory, "\\");
    strcat(dwnDiretory, argv[2]);


    printf("%s downloading...\n", dwnDiretory);
    urldown = URLDownloadToFile(NULL, argv[1], dwnDiretory, 0, NULL);

    switch(urldown){
        case E_OUTOFMEMORY:
            printf("download failed\n");
            break;
        case S_OK:
            printf("download sucess\n");
            break;
    }

    return 1;
}
