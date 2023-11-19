#include "headers.h"
int num_of_storage_servers = 5;
struct TrieNode *root;
struct storage_node *storage_server = NULL;

// void **handle_storage_connection_thread(void *arg)
// {
//     int client_socket = *((int *)arg);
//     handle_storage_connection(client_socket);
//     close(client_socket);
//     return NULL;
// }

#define LRULIMIT 5
#define ARRAYSIZE 100007
struct ll *head;
;

int size_cache = 0;

struct storage_node *cache[ARRAYSIZE];

struct ll
{
    int index;
    struct ll *next;
};

void pop_back(struct ll *head)
{
    struct ll *temp = head;
    while (temp->next->next != NULL)
    {
        temp = temp->next;
    }
    struct ll *temp1 = temp->next;
    temp->next = NULL;
    cache[temp1->index] = NULL;
    printf("Removed %d indexed cache\n", temp1->index);
    free(temp1);
}

void push_front(struct ll *head, int index)
{
    struct ll *temp = (struct ll *)malloc(sizeof(struct ll));
    temp->index = index;
    temp->next = head->next;
    head->next = temp;
    printf("Added %d indexed cache\n", index);
}

void pick_element(struct ll *head, int i)
{
    struct ll *temp = head;
    struct ll *node = head;
    while (node->next != NULL)
    {
        if (node->next->index == i)
        {
            printf("Updated\n");
            struct ll *tmp = node->next;
            struct ll *temp1 = node->next->next;
            node->next = temp1;
            tmp->next = head->next;
            head->next = tmp;
            break;
        }
        node = node->next;
    }
}

int det_index(char *path)
{
    int len = strlen(path);
    // printf("eifdbejb\n");
    int num = 31;
    int ret = 0;
    for (int i = 0; i < len; i++)
    {
        ret = (ret + path[i] * num) % ARRAYSIZE;
        num = (num * 31) % ARRAYSIZE;
    }
    return ret;
}

void init_cache()
{
    for (int i = 0; i < ARRAYSIZE; i++)
    {
        cache[i] = (struct storage_node *)malloc(sizeof(struct storage_node));
        cache[i] = NULL;
    }
}

void insert_cache(char *path, struct storage_node *st)
{
    int index = det_index(path);
    struct storage_node *st1 = (struct storage_node *)malloc(sizeof(struct storage_node));
    st1->storage_id = st->storage_id;
    strcpy(st1->storage_ip, st->storage_ip);
    st1->storage_port_for_client = st->storage_port_for_client;
    st1->storage_port_for_NM = st->storage_port_for_NM;
    printf("duvbwdb\n");
    {
        cache[index] = st1;
        size_cache++;
        if (size_cache > LRULIMIT)
        {
            push_front(head, index);
            pop_back(head);
        }
        else
        {
            printf("Added %d indexed cache\n", index);
            push_front(head, index);
        }
    }
}

struct storage_node *search_cache(char *path)
{
    int index = det_index(path);
    printf("Searching in cache\n");
    printf("%d\n", index);
    if (cache[index] == NULL)
    {
        return NULL;
    }
    else
    {
        printf("Found in cache\n");
        pick_element(head, index);
        return cache[index];
    }
}

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
        // printf("storage_ip: %s\n", root->storage_node->storage_ip);
        // printf("storage_port_for_NM: %d\n", root->storage_node->storage_port_for_NM);
        // printf("storage_port_for_client: %d\n", root->storage_node->storage_port_for_client);
        return root;
    }
    if ((path_len + 1) == strlen(path))
    {
        return NULL;
    }
    if (root->children[path[path_len + 1]] != NULL)
    {
        // printf("%c", root->children[path[path_len + 1]]->current_char);
        return search_path(root->children[path[path_len + 1]], path, path_len + 1);
    }
    else
    {
        return NULL;
    }
}

struct TrieNode *search(char *path, int path_len)
{
    // printf("%s\n", path);
    for (int i = 0; i < 128; i++)
    {
        // printf("%d\n", i);
        if (root->children[i] != NULL)
        {
            // printf("%c", root->children[i]->current_char);
            if (root->children[i]->current_char == path[0])
            {
                // printf("%c", root->children[i]->current_char);
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
        printf("Deleted\n");
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
        printf("Deleted\n");
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
    char **path_storage = (char **)malloc(sizeof(char *) * 1024);
    for (int i = 0; i < 1024; i++)
    {
        path_storage[i] = (char *)malloc(sizeof(char) * 1024);
    }
    int count_path = 0;
    // Receive data from the client
    struct storage_node *temp = (struct storage_node *)malloc(sizeof(struct storage_node));
    ssize_t bytes_received;
    int i = 0;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[bytes_received - 1] = '\0';
        //  Process received data (in this example, just print to console)
        if (i == 0)
        {

            strcpy(temp->storage_ip, buffer);
            i++;
            continue;
        }
        if (i == 1)
        {
            temp->storage_port_for_NM = atoi(buffer);
            i++;
            continue;
        }
        if (i == 2)
        {
            temp->storage_port_for_client = atoi(buffer);
            i++;
            continue;
        }
        // buffer[bytes_received - 1] = '\0'; // Null-terminate the received data
        insert_path(buffer, temp);
        strcpy(path_storage[count_path], buffer);
        count_path++;
        // printf("Received from client: %s\n", buffer);
    }
    int nn = recv(client_socket, buffer, sizeof(buffer), 0);
    buffer[nn] = '\0';
    printf("Received from storage: %s\n", buffer);
    while(count_path--)
    {
        path_storage[count_path][strlen(path_storage[count_path])] = '\0';
        delete(path_storage[count_path]);
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

void *handle_storage_connection_thread(void *arg)
{
    int client_socket = *((int *)arg);
    handle_storage_connection(client_socket);
    close(client_socket);
    return NULL;
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
    int k = 0;
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
        pthread_t stroage_thread_id;
        pthread_create(&stroage_thread_id, NULL, handle_storage_connection_thread, &client_socket);
        // handle_storage_connection(client_socket);
    }
    return NULL;
}

void give_command_to_nm(char *ip, int port, char *function, char *search_path)
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
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+] Connected to the server.\n");
    printf("Connected to server\n");
    while (1)
    {
        int n1 = recv(sock, buffer, sizeof(buffer), 0);
        if (n1 == 0)
        {
            continue;
        }
        buffer[n1] = '\0';
        if (strcmp(buffer, "Accepted") == 0)
        {
            break;
        }
    }
    send(sock, function, strlen(function), 0);
    usleep(1000);
    send(sock, search_path, strlen(search_path), 0);
    usleep(1000);
    while (1)
    {
        int n1 = recv(sock, buffer, sizeof(buffer), 0);
        if (n1 == 0)
        {
            continue;
        }
        buffer[n1] = '\0';
        if (strcmp(buffer, "STOP") == 0)
        {
            break;
        }
    }
    close(sock);
}

void copy_parallely(char *src_ip, char *dest_ip, int src_port, int dest_port, char *src_path, char *dest_path, int src_server_number, int src_port_for_nm, int dst_port_for_nm)
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
    addr.sin_port = src_port;
    addr.sin_addr.s_addr = inet_addr(src_ip);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+] Connected to the server %d for Copying.\n", src_server_number + 1);
    while (1)
    {
        int n1 = recv(sock, buffer, sizeof(buffer), 0);
        if (n1 == 0)
        {
            continue;
        }
        buffer[n1] = '\0';
        if (strcmp(buffer, "Accepted") == 0)
        {
            break;
        }
    }
    // printf("Connected to server\n");
    send(sock, "copy_from_you", strlen("copy_from_you"), 0);
    usleep(1000);
    char *sending_command = (char *)malloc(sizeof(char) * 1024);
    strcpy(sending_command, src_path);
    send(sock, sending_command, strlen(sending_command), 0);

    int sock1;
    struct sockaddr_in addr1;
    socklen_t addr_size1;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }

    memset(&addr1, '\0', sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = dest_port;
    addr1.sin_addr.s_addr = inet_addr(dest_ip);
    connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    printf("[+] Connected to the server for Pasting.\n");
    // printf("Connected to server\n");
    while (1)
    {
        int n1 = recv(sock1, buffer, sizeof(buffer), 0);
        if (n1 == 0)
        {
            continue;
        }
        buffer[n1] = '\0';
        if (strcmp(buffer, "Accepted") == 0)
        {
            break;
        }
    }

    send(sock1, "paste_to_you", strlen("paste_to_you"), 0);
    usleep(1000);
    char *copy_file = (char *)malloc(sizeof(char) * 1024);
    char *copy_file1 = (char *)malloc(sizeof(char) * 1024);
    strcpy(copy_file, src_path);
    strcpy(copy_file1, src_path);
    char *token = strtok(copy_file1, "/");
    strcpy(copy_file, token);
    while (1)
    {
        strcpy(copy_file, token);
        token = strtok(NULL, "/");
        if (token == NULL)
        {
            break;
        }
    }
    // strcat(dest_path, "/");
    strcat(dest_path, copy_file);
    send(sock1, dest_path, strlen(dest_path), 0);
    // usleep(1000);
    // printf("Will start to recieve from here\n");
    // send(sock, "bakwas", sizeof("bakwas"), 0);
    while (1)
    {
        int n1 = recv(sock, buffer, sizeof(buffer), 0);
        // send(sock1, buffer, strlen(buffer), 0);
        if (n1 == 0)
        {
            continue;
        }
        usleep(1000);
        buffer[n1] = '\0';
        char *if_stop = buffer + strlen(buffer) - 4;
        if (strcmp(if_stop, "STOP") == 0)
        {
            buffer[strlen(buffer) - 4] = '\0';
            char *tmp_buffer = (char *)malloc(sizeof(char) * 1024);
            strcpy(tmp_buffer, buffer);
            //  printf("%s\n", tmp_buffer);
            send(sock1, tmp_buffer, strlen(tmp_buffer), 0);
            send(sock1, "STOP", strlen("STOP"), 0);
            printf("Done\n");
            break;
        }
        //  printf("%s\n", buffer);
        send(sock1, buffer, strlen(buffer), 0);
    }
    close(sock);
}

// pthread_mutex_t lock1;

void execute_client_request(char *command, char *search_path, int sock)
{
    // pthread_mutex_lock(&lock1);
    int a = atoi(command);
    printf("%d\n", a);
    printf("%s\n", search_path);
    if (a == 1)
    {
        struct storage_node *srch_cache = search_cache(search_path);
        if (srch_cache != NULL)
        {
            // insert_cache(search_path,srch_cache);
            send(sock, "Found", strlen("Found"), 0);
            usleep(1000);
            char *ip_st = srch_cache->storage_ip;
            int port_st = srch_cache->storage_port_for_client;
            ip_st[strlen(ip_st)] = '\0';
            send(sock, ip_st, strlen(ip_st), 0);
            usleep(1000);
            char port_st_str[1024];
            sprintf(port_st_str, "%d", port_st);
            port_st_str[strlen(port_st_str)] = '\0';
            send(sock, port_st_str, strlen(port_st_str), 0);
            // pthread_mutex_unlock(&lock1);
        }
        else
        {
            struct TrieNode *node = search(search_path, 0);
            if (node)
            {
                send(sock, "Found", strlen("Found"), 0);
                printf("Found\n");
                usleep(1000);
                char *ip_st = node->storage_node->storage_ip;
                int port_st = node->storage_node->storage_port_for_client;
                ip_st[strlen(ip_st)] = '\0';
                send(sock, ip_st, strlen(ip_st), 0);
                usleep(1000);
                char port_st_str[1024];
                sprintf(port_st_str, "%d", port_st);
                port_st_str[strlen(port_st_str)] = '\0';
                send(sock, port_st_str, strlen(port_st_str), 0);
                insert_cache(search_path, node->storage_node);
                // printf("storage_ip: %s\n", node->storage_node->storage_ip);
                // printf("storage_port_for_NM: %d\n", node->storage_node->storage_port_for_NM);
                // printf("storage_port_for_client: %d\n", node->storage_node->storage_port_for_client);
            }
            else
            {
                send(sock, "No such file or directory", strlen("No such file or directory"), 0);
                usleep(1000);
                printf("No such file or directory\n");
            }
            // pthread_mutex_unlock(&lock1);
        }
    }
    if (a == 2)
    {
        usleep(1000);
        char function_exec[1024];
        int n1 = recv(sock, function_exec, sizeof(function_exec), 0);
        function_exec[n1] = '\0';
        printf("recv function -- %s\n", function_exec);
        // printf("%s\n", search_path);
        if (strcmp(function_exec, "DELETE") == 0)
        {
            struct storage_node *srch_cache = search_cache(search_path);
            if (srch_cache != NULL)
            {
                // insert_cache(search_path,srch_cache);
                char *ip_st = srch_cache->storage_ip;
                int port_st = srch_cache->storage_port_for_NM;
                ip_st[strlen(ip_st)] = '\0';
                // pthread_mutex_unlock(&lock1);
                send(sock, "Deleted", strlen("Deleted"), 0);
                usleep(1000);
                give_command_to_nm(ip_st, port_st, function_exec, search_path);
            }
            else
            {
                struct TrieNode *node = search(search_path, 0);
                if (node)
                {
                    char *ip_st = node->storage_node->storage_ip;
                    int port_st = node->storage_node->storage_port_for_NM;
                    ip_st[strlen(ip_st)] = '\0';
                    // insert_cache(search_path, node->storage_node);
                    delete (search_path);
                    // pthread_mutex_unlock(&lock1);
                    send(sock, "Deleted", strlen("Deleted"), 0);
                    give_command_to_nm(ip_st, port_st, function_exec, search_path);
                    // send(sock, "Deleted", strlen("Deleted"), 0);
                    // printf("storage_ip: %s\n", node->storage_node->storage_ip);
                    // printf("storage_port_for_NM: %d\n", node->storage_node->storage_port_for_NM);
                    // printf("storage_port_for_client: %d\n", node->storage_node->storage_port_for_client);
                }
                else
                {
                    send(sock, "No such file or directory", strlen("No such file or directory"), 0);
                    // pthread_mutex_unlock(&lock1);
                    printf("No such file or directory\n");
                }
            }
        }
        else if (strcmp(function_exec, "CREATE") == 0)
        {
            printf("CREATE\n");
            printf("----%s----\n", search_path);
            struct storage_node *srch_cache = search_cache(search_path);
            printf("aaa\n");
            if (srch_cache != NULL)
            {
                // insert_cache(search_path,srch_cache);
                printf("Path already exists\n");
                send(sock, "Path already exists", strlen("Path already exists"), 0);
                usleep(1000);
            }
            else
            {
                struct TrieNode *node = search(search_path, 0);
                if (node)
                {
                    // insert_cache(search_path, node->storage_node);
                    // send path already exists
                    send(sock, "Path already exists", strlen("Path already exists"), 0);
                    usleep(1000);
                }
                else
                {
                    // pthread_mutex_unlock(&lock1);
                    struct storage_node *temp = (struct storage_node *)malloc(sizeof(struct storage_node));
                    temp->storage_id = 0;
                    strcpy(temp->storage_ip, "127.0.0.1");
                    temp->storage_port_for_NM = 10202;
                    temp->storage_port_for_client = 1232;
                    insert_cache(search_path, temp);
                    insert_path(search_path, temp);
                    send(sock, "Path created", strlen("Path created"), 0);
                    usleep(1000);
                    // pthread_mutex_unlock(&lock1);
                    give_command_to_nm("127.0.0.1", 10202, function_exec, search_path);
                }
            }
        }
    }
    if (a == 3)
    {
        send(sock, "Accepted", strlen("Accepted"), 0);
        char *src_path = (char *)malloc(sizeof(char) * 1024);
        char *dest_path = (char *)malloc(sizeof(char) * 1024);

        char *token = strtok(search_path, "|");
        strcpy(src_path, token);
        token = strtok(NULL, "|");
        strcpy(dest_path, token);

        printf("%s | %s\n", src_path, dest_path);

        struct TrieNode *node = search(src_path, 0);
        if (node == NULL)
        {
            printf("No such file or directory for %s\n", src_path);
        }
        else
        {
            printf("storage_ip: %s\n", node->storage_node->storage_ip);
            int src_port = node->storage_node->storage_port_for_NM;
            char *src_ip = node->storage_node->storage_ip;
            int src_server_number = node->storage_node->storage_id;
            int src_port_for_nm = node->storage_node->storage_port_for_NM;
            src_ip[strlen(src_ip)] = '\0';

            node = search(dest_path, 0);
            if (node == NULL)
            {
                printf("No such file or directory for %s\n", dest_path);
                return;
            }
            int dest_port = node->storage_node->storage_port_for_NM;
            char *dest_ip = node->storage_node->storage_ip;
            int dest_server_number = node->storage_node->storage_id;
            int dest_port_for_nm = node->storage_node->storage_port_for_NM;
            dest_ip[strlen(dest_ip)] = '\0';
            // pthread_mutex_unlock(&lock1);
            copy_parallely(src_ip, dest_ip, src_port, dest_port, src_path, dest_path, src_server_number, src_port_for_nm, dest_port_for_nm);
            // send(sock, "STOP", strlen("STOP"), 0);
        }
    }
    printf("returning\n");
    usleep(10000);
    // send(sock, "STOP", strlen("STOP"), 0);
}
void handle_client_connection(int client_socket)
{
    while (1)
    {
        // pthread_mutex_lock(&lock);
        char buffer[4096];
        char search_path[4096];
        char command[4096];
        char check_path[4096];
        ssize_t bytes_received;
        int flag = 0;
        int i = 0;
        while (1)
        {
            usleep(1000);
            char buffer[4096];
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received == 0)
            {
                continue;
            }
            buffer[bytes_received] = '\0';
            if (i == 0)
            {
                strcpy(command, buffer);
                printf("Received from client: %s\n", buffer);
                i++;
                if (atoi(command) == 4)
                {
                    break;
                }
                continue;
            }
            printf("Received from client: %s\n", buffer);
            strcpy(search_path, buffer);
            // strcpy(check_path, buffer);
            // struct TrieNode *node = search(check_path, 0);
            // if (node)
            // {
            //     send(client_socket, "Found", strlen("Found"), 0);
            //     usleep(1000);
            //     printf("Found\n");
            // }
            // else if (atoi(command) != 3)
            // {
            //     send(client_socket, "Not_Found", strlen("Not_Found"), 0);
            //     usleep(1000);
            //     printf("Not Found\n");
            //     flag = 1;
            // }
            if (i == 1)
            {
                break;
            }
        }
        if (atoi(command) == 4)
        {
            break;
        }
        if (flag == 1)
        {
            continue;
        }
        execute_client_request(command, search_path, client_socket);
        printf("NEW CLIENT REQUEST\n");
    }
    printf("Client disconnected\n");
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
    int port = 5565;
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
    int k = 0;
    while (1)
    {
        if ((client_socket = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("Error accepting connection");
            continue;
        }
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // Handle the connection in a new function
        pthread_t client_thread_id;
        pthread_create(&client_thread_id, NULL, handle_client_connection_thread, &client_socket);
    }
    return NULL;
}

int main()
{
    init_cache();
    head = (struct ll *)malloc(sizeof(struct ll));
    root = getNode('*');
    //  pthread_mutex_init(&lock1, NULL);
    pthread_t listen_nm_thread_id;
    pthread_create(&listen_nm_thread_id, NULL, listen_nm_thread, NULL);

    pthread_t listen_client_thread_id;
    pthread_create(&listen_client_thread_id, NULL, listen_client, NULL);

    pthread_join(listen_nm_thread_id, NULL);
    // give_command_to_nm();
    pthread_join(listen_client_thread_id, NULL);
    return 0;
}