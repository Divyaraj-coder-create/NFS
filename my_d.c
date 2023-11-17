#include "headers.h"
#define BUFFER_SIZE 100

int port_for_nm;
int port_for_client;
char *ip_for_client = "";



void read_or_write(char* path,char* type,int client_socket)
{
    if(strcmp(type,"read")==0)
    {
        // write code to read contents of file with path 
        // and send it to client
        FILE *file = fopen(path, "r"); 
        if (file == NULL) 
        { 
            perror("Error opening file"); 
            exit(EXIT_FAILURE); 
        }

        
        char line_buffer[BUFFER_SIZE];

        // Read file line by line and send each line to the server
        while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) { 
            // printf("Sending: %s", line_buffer); 
            send(client_socket, line_buffer, strlen(line_buffer), 0); 
            usleep(100); 
        }
        
        strcpy(line_buffer,"STOP");

        send(client_socket, line_buffer, strlen(line_buffer), 0); 
        // Close the file
        fclose(file);
    }
    else if(strcmp(type,"write")==0)
    {

        FILE *file = fopen(path, "r"); 
        if (file == NULL) 
        { 
            perror("Error opening file"); 
            exit(EXIT_FAILURE); 
        }

        
        char line_buffer[BUFFER_SIZE];

        // Read file line by line and send each line to the server
        while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) { 
            // printf("Sending: %s", line_buffer); 
            send(client_socket, line_buffer, strlen(line_buffer), 0); 
            usleep(100); 
        }
        
        strcpy(line_buffer,"STOP");

        send(client_socket, line_buffer, strlen(line_buffer), 0); 
        // Close the file
        fclose(file);


    }
}



void send_file_contents(const char *filename, int socket)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line_buffer[BUFFER_SIZE];
    int kk = 0;
    // Read file line by line and send each line to the server
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL)
    {

        //  printf("Sending: %s", line_buffer);
        if (kk == 0)
        {
            ip_for_client = line_buffer;
            kk++;
        }
        else if (kk == 1)
        {
            port_for_nm = atoi(line_buffer);
            kk++;
        }
        else if (kk == 2)
        {
            port_for_client = atoi(line_buffer);
            kk++;
        }
        else
        {
            kk++;
        }
        send(socket, line_buffer, strlen(line_buffer), 0);
        usleep(100);
    }
    // Close the file
    fclose(file);
}

void handle_command_create(char **command)
{
    char *check_name = command[1];
    check_name[strlen(check_name)] = '\0';
    if (strcmp(check_name, "file") == 0)
    {
        char *file_name = command[2];
        FILE *file = fopen(file_name, "w");
        printf("File name: %s is created\n", file_name);
        fclose(file);
    }
    else if (strcmp(check_name, "folder") == 0)
    {
        char *folder_name = command[2];
        mkdir(folder_name, 0777);
        printf("Folder name: %s is created\n", folder_name);
    }
    else
    {
        printf("Invalid command\n");
    }
}

void handle_command_delete(char **command)
{
    char *check_name = command[1];
    check_name[strlen(check_name)] = '\0';
    if (strcmp(check_name, "file") == 0)
    {
        char *file_name = command[2];
        remove(file_name);
        printf("File name: %s is deleted\n", file_name);
    }
    else if (strcmp(check_name, "folder") == 0)
    {
        char *folder_name = command[2];
        rmdir(folder_name);
        printf("Folder name: %s is deleted\n", folder_name);
    }
    else
    {
        printf("Invalid command\n");
    }
}

void handle_command(char *command)
{
    printf("Handling command: %s\n", command);
    // split command with space
    char *input = (char *)malloc(4096);
    strcpy(input, command);
    char *token = (char *)malloc(4096);
    token = strtok(input, " ");
    char *tokens[4096];
    int i = 0;
    while (token != NULL)
    {
        tokens[i] = malloc(4096);
        strcpy(tokens[i], token);
        // tokens[i] = token;
        token = strtok(NULL, " ");
        // tokens[i][strlen(tokens[i])] = '\0';
        i++;
    }
    if (strcmp(tokens[0], "create") == 0)
    {
        handle_command_create(tokens);
    }
    else if (strcmp(tokens[0], "delete") == 0)
    {
        handle_command_delete(tokens);
    }
    else
    {
        printf("Invalid command\n");
    }
}

void handle_nm_connection(int client_socket)
{
    char buffer[1024];
    // Receive data from client
    while (1)
    {
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        buffer[n] = '\0';
        handle_command(buffer);
        break;
        if (n == 0)
        {
            printf("Client disconnected\n");
            break;
        }
    }

    
    close(client_socket);
}

void *listen_nm(void *vargp)
{
    int port = port_for_nm;
    char *ip = "127.0.0.1";
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    socklen_t addr_size;
    char buffer[1024];
    int n;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
    listen(server_sock, 5);
    printf("Listening...\n");
    addr_size = sizeof(client_addr);
    int client_socket;
    while (1)
    {
        // Accept a connection
        if ((client_socket = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("Error accepting connection");
            continue;
        }
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("------- nm_server connected -------\n");
        // Handle the connection in a new function
        handle_nm_connection(client_socket);
    }
    return NULL;
}

void handle_client_connection(int client_socket)
{
    char buffer[1024];
    // Receive data from client
    strcpy(buffer,"Accepted");
    send(client_socket, buffer, strlen(buffer), 0);
    while (1)
    {
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        if (n == 0)
        {
            printf("Client disconnected\n");
            break;
        }
    }
    
    close(client_socket);
}

void *listen_client(void *vargp)
{
    int port = port_for_client;
    char *ip = "127.0.0.1";
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    socklen_t addr_size;
    char buffer[1024];
    int n;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = 5569;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
    listen(server_sock, 5);
    printf("Listening...\n");
    addr_size = sizeof(client_addr);
    int client_socket;
    while (1)
    {
        // Accept a connection
        if ((client_socket = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("Error accepting connection");
            continue;
        }
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("------- client connected -------\n");
        // Handle the connection in a new function
        handle_client_connection(client_socket);
    }
    return NULL;
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
    addr.sin_port = 5566;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+] Connected to the server.\n");

    printf("Connected to server\n");
    // Send file contents to the server
    send_file_contents("server_1_list.txt", sock);

    close(sock);

    pthread_t listen_nm_thread_id;
    pthread_create(&listen_nm_thread_id, NULL, listen_nm, NULL);

    pthread_t listen_client_thread_id;
    pthread_create(&listen_client_thread_id, NULL, listen_client, NULL);

    pthread_join(listen_nm_thread_id, NULL);
    pthread_join(listen_client_thread_id, NULL);
    // Close the client socket
}