#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "headers.h"

void nm_connection(int port_for_nm, char *ip)
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
    printf("[+]TCP server socket created.\n");
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port_for_nm;
    addr.sin_addr.s_addr = inet_addr(ip);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the naming_server.\n");
}

int main()
{
    char *ip = "127.0.0.1";
    int port_for_nm = 5566;
    int port_for_client = 5567;
    struct storage_node *temp = storage_server;
    while (temp != NULL)
    {
        temp = temp->next;
    }
    temp->storage_id = 1;
    strcpy(temp->storage_ip, ip);
    temp->storage_port_for_NM = port_for_nm;
    temp->storage_port_for_client = port_for_client;
    temp->next = NULL;
    nm_connection(port_for_nm, ip);

    // int sock;
    // struct sockaddr_in addr;
    // socklen_t addr_size;
    // char buffer[1024];
    // int n;

    // sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (sock < 0)
    // {
    //     perror("[-]Socket error");
    //     exit(1);
    // }
    // printf("[+]TCP server socket created.\n");

    // memset(&addr, '\0', sizeof(addr));
    // addr.sin_family = AF_INET;
    // addr.sin_port = port;
    // addr.sin_addr.s_addr = inet_addr(ip);

    // connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    // printf("Connected to the naming_server.\n");
    // printf("entre your ip address: ");
    // fgets(buffer, 1024, stdin);
    // send(sock, buffer, strlen(buffer), 0);
    // // buffer[strlen(buffer)-1]='\0';
    // printf("entre your port for naming server: ");
    // fgets(buffer, 1024, stdin);
    // send(sock, buffer, strlen(buffer), 0);
    // // bezero(buffer, 1024);
    // printf("entre your port for client: ");
    // fgets(buffer, 1024, stdin);
    // send(sock, buffer, strlen(buffer), 0);
    // bezero(buffer, 1024);
    // List of Accessible Paths
    // while (1)
    // {
    //     bzero(buffer, 1024);
    //     printf("Client: ");
    //     fgets(buffer, 1024, stdin);
    //     send(sock, buffer, strlen(buffer), 0);
    //     bzero(buffer, 1024);
    //     recv(sock, buffer, sizeof(buffer), 0);
    //     printf("Server: %s\n", buffer);
    // }
    // close(sock);
    // printf("Disconnected from the server.\n");

    return 0;
}