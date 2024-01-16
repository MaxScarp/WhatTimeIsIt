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

#define REFID_LENGTH 4
#define TIME_BUF_MAX_SIZE 80
#define TIME_FORMAT_STRING "\%d-%m-%Y %H:%M:\%S"

typedef struct ntpPacket
{
    unsigned char leap_version_mode;
    unsigned char stratum;
    unsigned char poll;
    unsigned char precision;
    unsigned int root_delay;
    unsigned int root_dispersion;
    char reference_identifier[REFID_LENGTH];
    unsigned long long reference_timestamp;
    unsigned long long original_timestamp;
    unsigned long long receive_timestamp;
    unsigned long long transmit_timestamp;
} ntpPacket;

void build_ntp_packet(ntpPacket* packet)
{
    memset(packet, 0, sizeof(ntpPacket));

    packet->leap_version_mode = 0 << 6 | 4 << 3 | 3;
    packet->transmit_timestamp = (unsigned long long)time(NULL);

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

    ntpPacket ntpPacket;
    build_ntp_packet(&ntpPacket);

    struct sockaddr_in ntpServer;
    ntpServer.sin_family = AF_INET;
    ntpServer.sin_port = htons(123);
    inet_pton(AF_INET, "216.239.35.12", &ntpServer.sin_addr);

    int ntpServerSize = sizeof(ntpServer);
    sendto(sock, (const char*)&ntpPacket, sizeof(ntpPacket), 0, (struct sockaddr*)&ntpServer, ntpServerSize);
    recvfrom(sock, (char*)&ntpPacket, sizeof(ntpPacket), 0, NULL, NULL);

    ntpPacket.transmit_timestamp = ntohl(ntpPacket.transmit_timestamp);
    ntpPacket.transmit_timestamp -= ((unsigned long long)3600 * 24 * (365 * 70 + 17)) ;
    
    struct tm timeInfo;
    #ifdef _WIN32
        if(gmtime_s(&timeInfo, (const time_t*)&ntpPacket.transmit_timestamp) != 0)
        {
            fprintf(stderr, "Error in gmtime_s!\n");
            #ifdef _WIN32
                closesocket(sock);
            #else
                close(sock);
            #endif
            return -1;
        }
    #else
        gmtime_r((const time_t*)&ntpPacket.transmit_timestamp, &timeInfo);
    #endif

    char timeBuffer[TIME_BUF_MAX_SIZE];
    strftime(timeBuffer, sizeof(timeBuffer), TIME_FORMAT_STRING, &timeInfo);
    printf("Actual time is: %s. May differe by 1 hour depending on the system!\n", timeBuffer);

    #ifdef _WIN32
        closesocket(sock);
    #else
        close(sock);
    #endif
    
    return 0;
}
