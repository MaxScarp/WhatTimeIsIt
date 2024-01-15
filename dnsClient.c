#ifdef _WIN32
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

typedef struct dnsPacket
{
    unsigned short identification;
    unsigned short qr1_opCode4_aa1_tc1_rd1_ra1_z1_ad1_cd1_rCode4;
    unsigned long questions;
    unsigned long answerRRs;
    unsigned long authorityRRs;
    unsigned long additionaRRs;

} dnsPacket;

void build_dns_packet(dnsPacket* packet)
{
    memset(packet, 0, sizeof(dnsPacket));

    srand(time(NULL));

    packet->identification = rand() % 0xFFFF; // a random number between 0 and 65535
    packet->qr1_opCode4_aa1_tc1_rd1_ra1_z1_ad1_cd1_rCode4 = 0x100; //putting rd to 1 for recursion

    return;
}

int main(int argc, char** argv, char** envs)
{
    dnsPacket dnsPacket;
    build_dns_packet(&dnsPacket);

    printf("%llu", sizeof(unsigned long));

    return 0;
}