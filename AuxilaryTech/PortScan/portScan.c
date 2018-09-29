#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <time.h>
#include <winsock2.h>

clock_t start, end;

void usage(){
    printf("usage: portscan remoteAddr startport-endport\n");
    printf("example: this.exe 192.168.1.1 80-255\n");
}

int main(int argc, char *argv[]){
    float costtime;
    SOCKET sock;
    WSADATA wsa;
    struct sockaddr_in sin;
    char *host, *p;
    int i,startport, endport, res;

    if(argc!=3){
        usage();
        return 0;
    }

    p = argv[2];
    if(strstr(p, "-")){
        startport = atoi(p);
        for(;*p;)
            if(*(p++) == '-') break;
        endport = atoi(p);
    }

    if(startport<1 || endport > 65535)
        return 0;

    host = argv[1];
    
    WSAStartup(MAKEWORD(1,1), &wsa);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host);

    start = clock(); //begin record time

    for(i = startport; i < endport; i++){
        sock = socket(AF_INET, SOCK_STREAM, 0);
        sin.sin_port = htons(i);
        if(sock == INVALID_SOCKET) break;
        res = connect(sock, (struct sockaddr*)&sin, sizeof(sin));
        if(res == 0){
            printf("%s %d\n", host, i);
            closesocket(sock);
        }
    }

    end = clock(); //stop record
    costtime = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Cost time: %f (s)\n", costtime);
    WSACleanup();
    return 0;
}
