#include <iostream>
#include <thread>
#include <unistd.h>
#include <string.h>
#include "anet.h"

void thread_func(int sockClient)
{
    char readBuf[1024] = {0};
    char writeBuf[1024] = {0};
    int nRead;
    while ((nRead = anetRead(sockClient, readBuf, sizeof(readBuf) - 1)) > 0)
    {
        printf("TcpServer, Read From Client: %s\n", readBuf);
    }
    sprintf(writeBuf, "Hello From Server...");
    printf("Server Send: %s\n", writeBuf);
    int nWrite = 0;
    while ((nWrite = anetWrite(sockClient, writeBuf, strlen(writeBuf))) <= 0)
    {
        printf("nWrite: %d\n", nWrite);
        usleep(1000);
    }
    close(sockClient);
}

int main(int argc, char *argv[])
{
    char err[64];
    char serv_addr[64] = "0.0.0.0";
    int serv_port = 60000;
    if (argc > 1)
    {
        sprintf(serv_addr, argv[1]);
    }
    if (argc > 2)
    {
        serv_port = atoi(argv[2]);
    }

    int sock_serv = anetTcpServer(err, serv_port, serv_addr, 5);
    if (sock_serv == ANET_ERR)
    {
        printf("anetTcpServer error: %s\n", err);
        return sock_serv;
    }

    char ip_client[64];
    int port_client;
    int sock_client;
    while (1)
    {
        sock_client = anetTcpAccept(err, sock_serv, ip_client, sizeof(ip_client), &port_client);
        if (sock_client != ANET_ERR)
        {
            printf("Client Accepted, IP: %s, Port: %d\n", ip_client, port_client);
            // std::thread td(thread_func, sock_client);
            anetNonBlock(err, sock_client);
            thread_func(sock_client);
        }
        else
        {
            printf("Accept error: %s\n", err);
        }
    }
    return 0;
}