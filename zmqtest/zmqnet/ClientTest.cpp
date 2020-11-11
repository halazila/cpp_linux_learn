#include <iostream>
#include "ZmqClientEnd.h"

int main(int argc, char *argv[])
{
    ZmqClientEnd client;
    client.SetClientID("Administrator");
    client.Connect("127.0.0.1:57900");
    client.Send("hello", 5);
    std::cout << "send data: hello" << std::endl;
    char recvstr[128] = {0};
    int nrecv = client.Receive(recvstr, 128);
    if (nrecv > 127)
    {
        recvstr[127] = '\0';
    }
    else
    {
        recvstr[nrecv] = '\0';
    }
    std::cout << "recv data: " << recvstr << std::endl;
    return 0;
}