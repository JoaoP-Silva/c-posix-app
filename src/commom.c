#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>

void logExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int addrParse(const char *addrStr, const char *portStr, struct sockaddr_storage *storage)
{
    if (addrStr == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0)
        return -1;
    port = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, addrStr, &inaddr4.s_addr))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)(&storage);
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, addrStr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)(&storage);
        addr6->sin6_family = AF_INET;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize)
{
    int version;
    char addrStr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;
    if (addr->sa_family == AF_INET)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)(&addr);
        version = 4;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrStr, INET_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr4->sin_port);
    }
    else if (addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)(&addr);
        version = 6;
        if (!inet_ntop(AF_INET, &(addr6->sin6_addr), addrStr, INET6_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr6->sin6_port);
    }
    else
    {
        logExit("unknown protocol family.");
    }
    if (str)
    {
        snprintf(str, strSize, "IPv%d %s %hu", version, addrStr, port);
    }
}

int ServerSockInit(const char *proto, const char *portStr,
                   struct sockaddr_storage *storage)
{
    if (proto == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0){return -1;}
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (strcmp(proto, "v4") == 0)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)(&storage);
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    else if (strcmp(proto, "v6") == 0)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)(&storage);
        addr6->sin6_family = AF_INET;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}