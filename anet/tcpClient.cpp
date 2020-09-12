#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "anet.h"

using namespace std;

void thread_func(char *addr, int port)
{
    char err[128];
    char servFormat[64];
    anetFormatAddr(servFormat, sizeof(servFormat), addr, port);
    int sockfd = anetTcpNonBlockConnect(err, addr, port);
    if (sockfd != ANET_ERR)
    {
        auto tid = std::this_thread::get_id();
        char sendBuf[128] = {0};
        int nWrite;
        printf("Connect Server [%s] Success: %s\n", servFormat, err);
        sprintf(sendBuf, "Hello From Client");
        while ((nWrite = anetWrite(sockfd, sendBuf, strlen(sendBuf))) <= 0)
        {
            printf("nWrite: %d\n", nWrite);
            usleep(1000);
        }
    }
    else
    {
        printf("Connect Server [%s] Error: %s\n", servFormat, err);
    }
    char readBuf[128];
    int nRead(0);
    usleep(1000);
    while ((nRead = anetRead(sockfd, readBuf, sizeof(readBuf))) <= 0)
    {
        printf("nRead: %d\n", nRead);
        usleep(1000);
    }
    printf("%s\n", readBuf);
    close(sockfd);
}

int g_count = 1;

int main(int argc, char *argv[])
{
    std::vector<std::thread> vecThread;
    vecThread.reserve(g_count);

    char addr[64] = "127.0.0.1";
    int port = 60000;
    if (argc > 1)
    {
        sprintf(addr, argv[1]);
    }
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }

    for (size_t i = 0; i < g_count; i++)
    {
        vecThread.push_back(std::thread(thread_func, addr, port));
    }
    for (size_t i = 0; i < vecThread.size(); i++)
    {
        vecThread[i].join();
    }

    return 0;
}
