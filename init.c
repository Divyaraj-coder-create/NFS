#include "headers.h"

#define STORAGE_ID 1

char *namingServerIP = "127.0.0.1";
int namingServerPort = 10115;

char *storageServerIP = "127.0.0.1";
int storageServerPort = 10210;

void init_storage_server()
{
}

int main()
{
    printf("------------ STORAGE #%d INTERFACE ------------\n", STORAGE_ID);
    init_storage_server();
    return 0;
}