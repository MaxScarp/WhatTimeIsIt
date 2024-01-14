#ifdef _WIN32
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif
#include <stdio.h>

int main(int argc, char **argv)
{
    // this part is only required on Windows: it initializes the Winsock2 dll
    #ifdef _WIN32
        WSADATA wsa_data;
        if(WSAStartup (0x0202, &wsa_data))
        {
            printf("Error: Unable to initialize winsock2!\n");
            return -1;
        }
    #endif

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0)
    {
        printf("Error: Unable to initialize the UDP socket!\n");
        return -1;
    }
    printf("Socket %d created!\n", s);

    struct sockaddr_in sin;
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr); // this will create a big endian 32 bit address
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9999); // converts 9999 to big endian
    int sent_bytes = sendto(s, "CIAO", 4, 0, (struct sockaddr *)&sin, sizeof(sin));
    printf("Sent %d bytes via UDP!\n", sent_bytes);

    return 0;
}