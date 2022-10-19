#include "commom.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

    int main(int argc, char **argv)
{

    struct sockaddr_storage storage;
    if (ServerSockInit(argv[1], argv[2], &storage) != 0)
    {
        logExit("Parameter error");
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logExit("Error at socket creation");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (bind(s, addr, sizeof(storage)) != 0)
    {
        logExit("Error at bind");
    }
    if (listen(s, 10) != 0)
    {
        logExit("Error at listen");
    }
    char addrStr[BUFSIZ];
    addrToStr(addr, addrStr, BUFSIZ);
    printf("Bound to %s, waiting connection\n", addrStr);

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logExit("Accept error");
        }

        char caddrStr[BUFSIZ];
        addrToStr(caddr, caddrStr, BUFSIZ);
        printf("Connection from %s\n", caddrStr);

        char buf[BUFSIZ];
        memset(buf, 0, BUFSIZ);
        size_t count = recv(csock, buf, BUFSIZ - 1, 0);
        printf("[msg] %s, %d bytes: %s\n", caddrStr, (int)count, buf);

        sprintf(buf, "remote endpoint: %.1000s\n", caddrStr);
        count = send(csock, buf, strlen(buf) + 1, 0);
        if (count != strlen(buf) + 1)
        {
            logExit("Error at send message\n");
        }
        close(csock);
    }
    exit(EXIT_SUCCESS);
}