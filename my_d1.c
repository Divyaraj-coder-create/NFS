#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char *clientIP = "127.0.0.1";
int clientPort = 10110;
char *namingServerIP = "127.0.0.1";
int namingServerPort = 5565;
#define pathSIZE 300

void connection_to_storage_server_for_read_write(char *ip, int port, char *path, char *function)
{
    printf("------%s-----\n", ip);
    printf("------%d-----\n", port);
    printf("------%s-----\n", path);
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
    addr.sin_port = 5569;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+] Connected to the server.\n");
    char buffer1[1024];
    while (1)
    {
        int n = recv(sock, buffer1, sizeof(buffer1), 0);
        buffer1[n] = '\0';
        printf("%s\n", buffer1);
        if (strcmp(buffer1, "Accepted") == 0)
        {
            // printf("Accepted\n");
            break;
        }
    }

    send(sock, path, strlen(path), 0);
    //  printf("path sent\n");
    usleep(1000);
    memset(buffer, '\0', sizeof(buffer));
    strcpy(buffer, function);
    send(sock, buffer, strlen(buffer), 0);
    usleep(1000);
    if (strcmp(function, "READ") == 0)
    {
        while (1)
        {
            int n = recv(sock, buffer, sizeof(buffer), 0);
            buffer[n] = '\0';
            if (strcmp(buffer, "STOP") == 0)
            {
                printf("----DONE----\n");
                break;
            }
            printf("%s\n", buffer);
            memset(buffer, '\0', sizeof(buffer));
        }
        close(sock);
    }
    else if (strcmp(function, "WRITE") == 0)
    {
        printf("Enter choice: 0 for write, 1 for append\n");
        char choice[10];
        memset(choice, '\0', sizeof(choice));
        fgets(choice, 10, stdin);
        send(sock, choice, sizeof(choice), 0);
        int a = atoi(choice);
        if (a == 0)
        {
        printf("Enter string to write: \n");
        char sTR[1024];
        fgets(sTR, 1024, stdin);
        send(sock, sTR, sizeof(sTR), 0);
        close(sock);
        }
        else if (a == 1)
        {
            printf("Enter string to append: \n");
            char sTR[1024];
            fgets(sTR, 1024, stdin);
            send(sock, sTR, sizeof(sTR), 0);
            close(sock);
        }
    }

    // char return_val[1024];
    // memset(return_val, '\0', sizeof(return_val));
    // while (1)
    // {
    //     int n = recv(sock, return_val, sizeof(return_val), 0);
    //     return_val[n] = '\0';
    //     // printf("%s\n", return_val);
    //     if (strcmp(return_val, "Done") == 0)
    //     {
    //         printf("Done\n");
    //         break;
    //     }
    // }

    // printf("%s\n", buffer);
    close(sock);
}

void handle_read_write(int sock, char *path, char *function)
{
    char buffer[1024];
    // memset(buffer, '\0', sizeof(buffer));
    char ip_for_stor[1024];
    int port_for_stor;
    int c = 2;
    while (c--)
    {
        int n = recv(sock, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
        if (c == 1)
        {
            strcpy(ip_for_stor, buffer);
            memset(buffer, '\0', sizeof(buffer));
        }
        else if (c == 0)
        {
            port_for_stor = atoi(buffer);
            memset(buffer, '\0', sizeof(buffer));
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    // close(sock);
    // printf("------%s-----\n", ip_for_stor);
    // printf("------%d-----\n", port_for_stor);
    connection_to_storage_server_for_read_write(ip_for_stor, port_for_stor, path, function);
}

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

    // send(sock, "Client Request To Connect", strlen("Client Request To Connect"), 0);
    // memset(buffer, '\0', sizeof(buffer));
    // recv(sock, buffer, 1024, 0);
    //  if (strcmp(buffer, "Accepted") != 0)
    //  {
    //      printf("Server is experiencing exceptionally high demand. Please return after some time\n");
    //      exit(1);
    //  }

    printf("The Client must provide the path to the file/directory wherever applicable.\n");
    printf("------------ CLIENT INTERFACE ------------\n");
    while (1)
    {
        printf("Our Server Provied The Following Functionalities :\n");
        printf("press 1 for Reading, Writing, and Retrieving Information about Files\n");
        printf("press 2 for Creating and Deleting Files and Folders\n");
        printf("press 3 for Copying Files/Directories Between Storage Servers\n");
        char *choice = (char *)malloc(10 * sizeof(char));
        memset(choice, '\0', 10);
        printf("Enter your choice : ");
        fgets(choice, 10, stdin);
        send(sock, choice, strlen(choice), 0);
        printf("choice ---- %d\n", atoi(choice));
        if (atoi(choice) == 1)
        {
            char buffer[1024];
            char *function = (char *)malloc(100 * sizeof(char));
            char *path = (char *)malloc(pathSIZE * sizeof(char));
            memset(path, '\0', pathSIZE);
            memset(function, '\0', 100);
            printf("Enter the function with path of the file you want to access : ");
            fgets(buffer, 1024, stdin);
            buffer[strlen(buffer) - 1] = '\0';
            char *token = strtok(buffer, " ");
            strcpy(function, token);
            function[strlen(function)] = '\0';
            token = strtok(NULL, " ");
            strcpy(path, token);
            // fgets(function, 100, stdin);
            //  printf("Enter the path of the file you want to access : ");
            //  fgets(path, pathSIZE, stdin);
            printf("function : %s\n", function);
            printf("path : %s\n", path);
            send(sock, path, strlen(path), 0);
            memset(buffer, '\0', sizeof(buffer));
            handle_read_write(sock, path, function);
        }
        break;
        // FUNCTIONALITIES LIST TO BE WRITTEN HERE
        // STARTING WITH ONLY SEDNING PATH TO SERVER
        // char *operation = (char *)malloc(100 * sizeof(char));
        // memset(operation, '\0', 100);
        // // set operation to if-else condition
        // send(sock, operation, strlen(operation), 0);
    }
    printf("Disconnected from the server.\n");
    close(sock);

    return 0;
}