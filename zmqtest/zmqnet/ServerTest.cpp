#include "ZmqServerEnd.h"

int main(int argc, char *argv[])
{
    ZmqServerEnd server;
    server.Start();
    server.Join();
    return 0;
}