#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdbool.h>
#define BUFFER_SIZE 100

int port_for_nm = 10204;
int port_for_client = 1234;
char *ip_for_client = "";

void recv_file_contents(char *path, int client_socket)
{
    // printf("path to paste to : %s\n", path);
    FILE *file = fopen(path, "wb"); // replace with the path of the file you want to send
    if (file == NULL)
        perror("ERROR opening file");

    char buffer[BUFFER_SIZE];
    int n;
    // Read and send the file
    while (1)
    {
        n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n == 0)
        {
            continue;
        }
        usleep(1000);
        buffer[n] = '\0';
        char *if_stop = buffer + strlen(buffer) - 4;
        if (strcmp(if_stop, "STOP") == 0)
        {
            buffer[strlen(buffer) - 4] = '\0';
            char *tmp_buffer = (char *)malloc(sizeof(char) * 1024);
            strcpy(tmp_buffer, buffer);
            // printf("Line recieved from NM: %s\n", tmp_buffer);
            fwrite(tmp_buffer, 1, n - 4, file);
            // printf("Done\n");
            break;
        }
        // printf("Line recieved from NM: %s\n", buffer);
        fwrite(buffer, 1, n, file);
    }

    // printf("File received successfully.\n");

    // Cleanup and close socket
    fclose(file);
}

void send_contents(char *path, int client_socket)
{
    // send(port_for_nm, "Sending", strlen("Sending"), 0);
    // usleep(100);
    // char *bakwas;
    // recv(port_for_nm, bakwas, 100, 0);
    // printf("path to copy from : %s\n", path);
    FILE *file = fopen(path, "rb"); // replace with the path of the file you want to send
    if (file == NULL)
        perror("ERROR opening file");

    char buffer[10 * BUFFER_SIZE];
    size_t bytesRead;

    // Read and send the file
    while ((bytesRead = fread(buffer, 1, 10 * BUFFER_SIZE, file)) > 0)
    {
        // printf("Sending: %s\n", buffer);
        send(client_socket, buffer, bytesRead, 0);
        // recv(client_socket, buffer, sizeof(buffer), 0);
        memset(buffer, '\0', sizeof(buffer));
        // usleep(100);
    }

    strcpy(buffer, "STOP");
    // printf("Sending: %s\n", buffer);
    send(client_socket, buffer, strlen(buffer), 0);

    printf("File sent successfully.\n");

    // Cleanup and close socket
    fclose(file);
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

void handle_command(char *function, char *path, int client_socket)
{
    printf("function:---%s---\n", function);
    if (strcmp(function, "CREATE") == 0)
    {
        char last_char = path[strlen(path) - 1];
        printf("last_char: %c\n", last_char);
        if (last_char != '/')
        {
            char *file_name = path;
            FILE *file = fopen(file_name, "w");
            printf("File name: %s is created\n", file_name);
            fclose(file);
        }
        else if (last_char == '/')
        {
            char *folder_name = path;
            mkdir(folder_name, 0777);
            printf("Folder name: %s is created\n", folder_name);
        }
    }
    else if (strcmp(function, "DELETE") == 0)
    {
        char last_char = path[strlen(path) - 1];
        if (last_char != '/')
        {
            char *file_name = path;
            remove(file_name);
            printf("File name: %s is deleted\n", file_name);
        }
        else if (last_char == '/')
        {
            char *folder_name = path;
            rmdir(folder_name);
            printf("Folder name: %s is deleted\n", folder_name);
        }
    }
    else if (strcmp(function, "copy_from_you") == 0)
    {
        printf("copy_from_you\n");
        send_contents(path, client_socket);
    }
    else if (strcmp(function, "paste_to_you") == 0)
    {
        printf("paste_to_you\n");
        recv_file_contents(path, client_socket);
    }
    else
    {
        printf("Invalid command\n");
    }
    send(client_socket, "STOP", strlen("STOP"), 0);
}

void handle_nm_connection(int client_socket)
{
    char buffer[1024];
    char copy_buffer[1024];
    // Receive data from client
    strcpy(buffer, "Accepted");
    buffer[strlen(buffer)] = '\0';
    send(client_socket, buffer, strlen(buffer), 0);
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    usleep(100);
    buffer[n] = '\0';
    strcpy(copy_buffer, buffer);
    printf("Received: %s\n", buffer);
    int n1 = recv(client_socket, buffer, sizeof(buffer), 0);
    usleep(100);
    buffer[n1] = '\0';
    printf("Received: %s\n", buffer);
    handle_command(copy_buffer, buffer, client_socket);
    printf("------- nm_server disconnected -------\n");
    close(client_socket);
}

void *handle_nm_connection_thread(void *arg)
{
    int client_socket = *((int *)arg);
    handle_nm_connection(client_socket);
    close(client_socket);
    return NULL;
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
        pthread_t nm_thread_id;
        pthread_create(&nm_thread_id, NULL, handle_nm_connection_thread, (void *)&client_socket);
        // handle_nm_connection(client_socket);
    }
    return NULL;
}

void get_permissions(mode_t st_mode, char *permissions)
{
    mode_t permission_bits = st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    sprintf(permissions, "%o", permission_bits);
}

void read_or_write(char *path, char *type, int client_socket)
{
    if (strcmp(type, "READ") == 0)
    {
        printf("--READ--\n");
        FILE *file = fopen(path, "r");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        char line_buffer[BUFFER_SIZE];
        // Read file line by line and send each line to the server
        while (fgets(line_buffer, sizeof(line_buffer), file) != NULL)
        {
            printf("Sending: %s", line_buffer);
            line_buffer[strlen(line_buffer)] = '\0';
            send(client_socket, line_buffer, strlen(line_buffer), 0);
            usleep(100);
        }
        printf("a");
        // usleep(100);
        fclose(file);
        // usleep(1000);
        strcpy(line_buffer, "STOP");
        line_buffer[strlen(line_buffer)] = '\0';
        printf("Sending:--%s--", line_buffer);
        send(client_socket, line_buffer, strlen(line_buffer), 0);
        // Close the file
    }
    else if (strcmp(type, "WRITE") == 0)
    {
        char choice[1024];
        int n = recv(client_socket, choice, sizeof(choice), 0);
        // choice[n] = '\0';
        printf("choice:--%s--\n", choice);
        int choice1 = atoi(choice);
        if (choice1 == 1)
        {
            FILE *file;
            int a = 0;
            file = fopen(path, "a");
            int fd = fileno(file);
            while (1)
            {
                int ff = flock(fd, LOCK_EX | LOCK_NB);
                if (ff != -1)
                {
                    break;
                }
                if (a == 0 && ff == -1)
                {
                    printf("Waiting for file to be free\n");
                    a++;
                }
            }
            send(client_socket, "Accepted", strlen("Accepted"), 0);
            usleep(100);
            while (1)
            {
                char string[1024];
                int n1 = recv(client_socket, string, sizeof(string), 0);
                if (n1 == 0)
                {
                    continue;
                }
                string[n1] = '\0';
                printf("string: %s\n", string);
                if (strcmp(string, "STOP") == 0)
                {
                    break;
                }
                if (file == NULL)
                {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                fprintf(file, "%s\n", string);
            }
            flock(fd, LOCK_UN);
            fclose(file);
        }
        else if (choice1 == 0)
        {
            // fclose(file);
            // FILE *file1 = fopen(path, "a");

            FILE *file1;
            int a = 0;
            file1 = fopen(path, "w");
            int fd = fileno(file1);
            while (1)
            {
                int ff = flock(fd, LOCK_EX | LOCK_NB);
                if (ff != -1)
                {
                    break;
                }
                if (a == 0 && ff == -1)
                {
                    printf("Waiting for file to be free\n");
                    a++;
                }
            }
            send(client_socket, "Accepted", strlen("Accepted"), 0);
            while (1)
            {
                char string[1024];
                int n1 = recv(client_socket, string, sizeof(string), 0);
                if (n1 == 0)
                {
                    continue;
                }
                string[n1] = '\0';
                printf("string: %s\n", string);
                if (strcmp(string, "STOP") == 0)
                {
                    break;
                }
                if (file1 == NULL)
                {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                fprintf(file1, "%s\n", string);
            }
            flock(fd, LOCK_UN);
            fclose(file1);
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    else if (strcmp(type, "DETAILS") == 0)
    {
        // give size of file or folder with given path
        struct stat st;
        stat(path, &st);
        int size = st.st_size;
        char size_str[1024];
        sprintf(size_str, "%d", size);
        send(client_socket, size_str, strlen(size_str), 0);
        usleep(10000);
        char permissions[1024];
        get_permissions(st.st_mode, permissions);
        printf("permissions: %s\n", permissions);
        send(client_socket, permissions, strlen(permissions), 0);
    }
}

void handle_client_connection(int client_socket)
{
    char buffer[1024];
    char function[1024];
    char path[1024];
    // Receive data from client
    strcpy(buffer, "Accepted");
    buffer[strlen(buffer)] = '\0';
    send(client_socket, buffer, strlen(buffer), 0);
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    usleep(100);
    buffer[n] = '\0';
    // printf("Received: %s\n", buffer);
    strcpy(path, buffer);
    memset(buffer, '\0', sizeof(buffer));
    int n1 = recv(client_socket, buffer, sizeof(buffer), 0);
    usleep(100);
    buffer[n1] = '\0';
    // printf("Received: %s\n", buffer);
    strcpy(function, buffer);
    path[strlen(path)] = '\0';
    function[strlen(function)] = '\0';
    printf("path: %s\n", path);
    printf("function: %s\n", function);
    read_or_write(path, function, client_socket);
    printf("------- client disconnected -------\n");
    close(client_socket);
}

void *handle_client_connection_thread(void *arg)
{
    int client_socket = *((int *)arg);
    handle_client_connection(client_socket);
    close(client_socket);
    return NULL;
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
    server_addr.sin_port = port_for_client;
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
        pthread_t client_thread_id;
        pthread_create(&client_thread_id, NULL, handle_client_connection_thread, (void *)&client_socket);
        // handle_client_connection(client_socket);
    }
    return NULL;
}

void *intial_connection(void *argp)
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
    send_file_contents("server_2_list.txt", sock);
    printf("if you want to exit WRITE close_storage :\n");
    while (1)
    {
        char command[1024];
        scanf("%s", command);
        printf("command: %s\n", command);
        if (strcmp(command, "close_storage") == 0)
        {
            send(sock, command, strlen(command), 0);
            printf("Closing storage\n");
            exit(0);
            break;
        }
        else
        {
            printf("Invalid command\n");
        }
    }
    close(sock);
    return NULL;
}

int main()
{
    pthread_t intial_connection_thread_id;
    pthread_create(&intial_connection_thread_id, NULL, intial_connection, NULL);

    pthread_t listen_nm_thread_id;
    pthread_create(&listen_nm_thread_id, NULL, listen_nm, NULL);

    pthread_t listen_client_thread_id;
    pthread_create(&listen_client_thread_id, NULL, listen_client, NULL);


    pthread_join(listen_nm_thread_id, NULL);
    pthread_join(listen_client_thread_id, NULL);
    pthread_join(intial_connection_thread_id, NULL);
    // Close the client socket
}