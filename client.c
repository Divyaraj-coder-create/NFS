#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char *clientIP = "127.0.0.1";
int clientPort = 10110;

char *namingServerIP = "127.0.0.1";
int namingServerPort = 10115;

#define pathSIZE 300

int main()
{

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = namingServerPort;
    addr.sin_addr.s_addr = inet_addr(namingServerIP);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+] Connected to the server.\n");
    send(sock, "Client Request To Connect", strlen("Client Request To Connect"), 0);
    memset(buffer, '\0', sizeof(buffer));
    recv(sock, buffer, 1024, 0);
    if (strcmp(buffer, "Accepted") != 0)
    {
        printf("Server is experiencing exceptionally high demand. Please return after some time\n");
        exit(1);
    }
    printf("The Client must provide the path to the file/directory wherever applicable.\n");
    printf("------------ CLIENT INTERFACE ------------\n");
    while (1)
    {
        printf("Our Server Provied The Following Functionalities :\n");
        // FUNCTIONALITIES LIST TO BE WRITTEN HERE
        // STARTING WITH ONLY SEDNING PATH TO SERVER
        // char *operation = (char *)malloc(100 * sizeof(char));
        // memset(operation, '\0', 100);
        // // set operation to if-else condition
        // send(sock, operation, strlen(operation), 0);
        char *path = (char *)malloc(pathSIZE * sizeof(char));
        memset(path, '\0', pathSIZE);
        printf("Enter the path of the file you want to access : ");
        scanf("%s", path);
        send(sock, path, strlen(path), 0);
    }
    printf("Disconnected from the server.\n");
    close(sock);

    return 0;
}