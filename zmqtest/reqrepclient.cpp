#include "zmq.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://127.0.0.1:5559");

    char sendstr[32] = {0};
    char recvstr[32] = {0};
    for (int i = 0; i < 10; i++)
    {
        memset(sendstr, 0, sizeof(sendstr));
        sprintf(sendstr, "Hello_%d", i);
        std::cout << "send data: " << sendstr << std::endl;
        zmq_send(requester, sendstr, strlen(sendstr), 0);
        memset(recvstr, 0, sizeof(sendstr));
        int nrecv = zmq_recv(requester, recvstr, 31, 0);
        if (nrecv > 31)
        {
            recvstr[31] = '\0';
        }
        std::cout << "recv data: " << recvstr << std::endl;
    }

    return 0;
}