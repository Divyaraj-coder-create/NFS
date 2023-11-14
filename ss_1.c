#include "headers.h"
#define BUFFER_SIZE 100

void send_file_contents(const char *filename, int socket)
{
    FILE *file = fopen(filename, "r");
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
        send(socket, line_buffer, strlen(line_buffer), 0);
        usleep(100);
    }
    // Close the file
    fclose(file);
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
    
    // Close the client socket
    close(sock);
}