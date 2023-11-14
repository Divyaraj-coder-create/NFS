#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "strings.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "netdb.h"
#include "arpa/inet.h"
#define max_size 4096



extern int socket_naming_server;
extern int socket_storage_server[max_size];
extern int socket_client;