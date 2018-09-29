#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <winsock2.h>

void checkIp(){
    WSADATA wsa;
    char name[255], *ip;
    PHOSTENT hostinfo;

    WSAStartup(MAKEWORD(2,0), &wsa);

    gethostname(name, sizeof(name));
    hostinfo = gethostbyname(name);
    ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
    printf("host: %s\n", name);
    printf("addr: %s\n", ip);

    WSACleanup();
}

int main(){
    checkIp();
    return 0;
}
