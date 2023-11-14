#include "headers.h"
int num_of_storage_servers = 5;
struct TrieNode *root;
struct storage_node *storage_server = NULL;

struct TrieNode *getNode(char current_char)
{
    struct TrieNode *pNode = NULL;
    pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    if (pNode)
    {
        int i;
        pNode->isEndOfPath = 0;
        pNode->valid = 0;
        pNode->storage_node = NULL;
        pNode->current_char = current_char;
        pNode->isDirectory = 0;
        for (i = 0; i < 128; i++)
        {
            pNode->children[i] = NULL;
        }
    }
    return pNode;
}

void insert_path(const char *path, struct storage_node *storage_node)
{
    struct TrieNode *current = root;
    while (*path)
    {
        if (current->children[*path] == NULL)
        {
            current->children[*path] = getNode(path[0]);
        }
        current = current->children[*path];
        path++;
    }
    current->isEndOfPath = 1;
    current->storage_node = storage_node;
    current->valid = 1;
}

void print_trie(struct TrieNode *root)
{
    // int is_end_of_path = 0;
    if (root->isEndOfPath)
    {
        printf("\n");
        // is_end_of_path = 1;
    }
    for (int i = 0; i < 128; i++)
    {
        if (root->children[i])
        {
            printf("%c", i);
            print_trie(root->children[i]);
        }
    }
}

struct TrieNode *search_path(struct TrieNode *root, char *path, int path_len)
{
    // printf("%c", root->children[path[path_len + 1]]->current_char);
    if (root->isEndOfPath && (path_len + 1) == strlen(path))
    {
        printf("\n");
        return root;
    }
    if ((path_len + 1) == strlen(path))
    {
        return NULL;
    }
    if (root->children[path[path_len + 1]] != NULL)
    {
        printf("%c", root->children[path[path_len + 1]]->current_char);
        return search_path(root->children[path[path_len + 1]], path, path_len + 1);
    }
    else
    {
        return NULL;
    }
}

struct TrieNode *search(char *path, int path_len)
{
    for (int i = 0; i < 128; i++)
    {
        if (root->children[i] != NULL)
        {
            if (root->children[i]->current_char == path[0])
            {
                printf("%c", root->children[i]->current_char);
                return search_path(root->children[i], path, 0);
            }
        }
    }
    return NULL;
}

int delete_node(struct TrieNode *node, char *path, int current)
{
    if (current + 1 == strlen(path))
    {
        node->isEndOfPath = 0;
        return 1;
    }
    delete_node(node->children[path[current + 1]], path, current + 1);
    return 1;
}

void delete_recursive(struct TrieNode *node)
{
    if (node->isEndOfPath)
    {
        // free(node);
        node->isEndOfPath = 0;
        return;
    }
    for (int i = 0; i < 128; i++)
    {
        if (node->children[i])
        {
            delete_recursive(node->children[i]);
        }
    }
    // free(node);
    node->isEndOfPath = 0;
    return;
}

int delete_directory(struct TrieNode *node)
{
    delete_recursive(node);
    return 1;
}

int delete(char *path)
{
    if (search(path, 0))
    {
        struct TrieNode *node = search(path, 0);
        if (node->isDirectory)
        {
            return delete_directory(node);
        }
        return delete_node(root->children[path[0]], path, 0);
    }
    else
    {
        return 0;
    }
}

void client_connection(int client_sock)
{
    char buffer[1024];
    while (1)
    {
        bzero(buffer, 1024);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);
        bzero(buffer, 1024);
        printf("Server: ");
        fgets(buffer, 1024, stdin);
        send(client_sock, buffer, strlen(buffer), 0);
    }
}

void handle_storage_connection(int client_socket)
{
    char buffer[4096];
    // Receive data from the client
    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
    {
        // Process received data (in this example, just print to console)
        buffer[bytes_received - 1] = '\0'; // Null-terminate the received data
        insert_path(buffer, NULL);
        printf("Received from client: %s", buffer);
    }
    if (bytes_received == 0)
    {
        printf("Client disconnected\n");
    }
    else if (bytes_received == -1)
    {
        perror("Error receiving data from client");
    }
    close(client_socket);
}

void *listen_nm_thread(void *args)
{
    int port = 5566;
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
        // Handle the connection in a new function
        handle_storage_connection(client_socket);
        break;
    }
    return NULL;
}

int main()
{
    root = getNode('*');
    pthread_t listen_nm_thread_id;
    pthread_create(&listen_nm_thread_id, NULL, listen_nm_thread, NULL);
    // sleep(5);
    pthread_join(listen_nm_thread_id, NULL);
    // char *search_path = "sidd";
    //  search_path[strlen(search_path)] = '\0';
    //  int a = search(search_path, 0);
    //  printf("%d\n", a);
    delete ("neel_amrutia");
    printf("\n");
    print_trie(root);

    // int port = 5566;
    // int server_sock, client_sock;
    // struct sockaddr_in server_addr, client_addr;
    // socklen_t addr_size;
    // char buffer[1024];
    // int n;
    // server_sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (server_sock < 0)
    // {
    //     perror("[-]Socket error");
    //     exit(1);
    // }
    // printf("[+]TCP server socket created.\n");
    // memset(&server_addr, '\0', sizeof(server_addr));
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = port;
    // server_addr.sin_addr.s_addr = inet_addr(ip);
    // n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // if (n < 0)
    // {
    //     perror("[-]Bind error");
    //     exit(1);
    // }
    // printf("[+]Bind to the port number: %d\n", port);
    // listen(server_sock, 5);
    // printf("Listening...\n");
    // addr_size = sizeof(client_addr);
    // char connection_type[1024];
    // bzero(connection_type, 1024);
    // printf("entre your connection type: ");
    // scanf("%s", connection_type);
    // if (strcmp(connection_type, "client") == 0)
    // {
    //     client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    //     printf("[+]Client connected.\n");
    //     client_connection(client_sock);
    //     close(client_sock);
    //     printf("[+]Client disconnected.\n\n");
    // }
    // else if (strcmp(connection_type, "storage") == 0)
    // {
    // client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    // printf("[+]Storage connected.\n");
    // printf("[+]Storage disconnected.\n\n");
    // }

    return 0;
}