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

#define DOMAIN_MAX_LENGTH 256
#define DOMAIN_DELIMITER "."

typedef struct dnsPacket
{
    unsigned short identification;
    unsigned short qr1_opCode4_aa1_tc1_rd1_ra1_z1_ad1_cd1_rCode4;
    unsigned short totalQuestions;
    unsigned short totalAnswerRRs;
    unsigned short totalAuthorityRRs;
    unsigned short totalAdditionaRRs;
    unsigned long questions;
    unsigned long answerRRs;
    unsigned long long authorityRRs;
    unsigned long long additionaRRs;
} dnsPacket;

void build_dns_packet(dnsPacket* packet, const char* domain)
{
    memset(packet, 0, sizeof(dnsPacket));

    srand(time(NULL));

    packet->identification = rand() % 0xFFFF; // a random number between 0 and 65535
    printf("Identification : %u\n", packet->identification);

    packet->qr1_opCode4_aa1_tc1_rd1_ra1_z1_ad1_cd1_rCode4 = htons(0x100); //putting rd to 1 for recursion
    packet->totalQuestions = htons(1); //Number of questions

    char domainBuffer[DOMAIN_MAX_LENGTH];
    if(strncpy_s(domainBuffer, sizeof(domainBuffer), domain, sizeof(domainBuffer) - 1) != 0)
    {
        return;
    }
    domainBuffer[DOMAIN_MAX_LENGTH - 1] = '\0';

    char* nextToken;
    char* token = strtok_s(domainBuffer, DOMAIN_DELIMITER, &nextToken);

    while(token != NULL)
    {
        size_t tokenLength = strlen(token);
        printf("%s", token);
        strcpy((char*)packet->questions, token);
        token = strtok_s(NULL, DOMAIN_DELIMITER, &nextToken);
    }

    return;
}

int main(int argc, char** argv, char** envs)
{
    #ifdef _WIN32
        WSADATA wsaData;
        if(WSAStartup(0x0202, &wsaData))
        {
            fprintf(stderr, "Error: Unable to initialize winsock2!\n");
            return -1;
        }
    #endif

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
    {
        fprintf(stderr, "Error: Unable to initialize the UDP socket!\n");
        return -1;
    }
    printf("Socket %d created!\n", sock);

    dnsPacket dnsPacket;
    build_dns_packet(&dnsPacket, "aiv01.it");
    //if(dnsPacket.)

    struct sockaddr_in dnsServer;
    dnsServer.sin_family = AF_INET;
    dnsServer.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &dnsServer.sin_addr);
    
    sendto(sock, (const char*)&dnsPacket, sizeof(dnsPacket), 0, (struct sockaddr*)&dnsServer, sizeof(dnsServer));
    return 0;
}