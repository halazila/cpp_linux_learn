#include "zmq.hpp"
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t responder(context, ZMQ_REP);

    responder.bind("tcp://*:5559");

    char sendstr[32] = {0};
    char recvstr[32] = {0};
    zmq::message_t message(32);
    while (1)
    {
        memset(recvstr, 0, sizeof(sendstr));
        int nrecv = zmq_recv(responder, recvstr, 31, 0);
        if (nrecv > 31)
        {
            recvstr[31] = '\0';
        }
        std::cout << "recv bytes: " << nrecv << ", recv data: " << recvstr << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int i = 0;
        for (i = 0; i < 31; i++)
        {
            if (isdigit(recvstr[i]))
            {
                break;
            }
        }
        memset(sendstr, 0, sizeof(sendstr));
        sprintf(sendstr, "World_%s", recvstr + i);
        std::cout << "send data: " << sendstr << std::endl;
        zmq_send(responder, sendstr, strlen(sendstr), 0);
    }

    return 0;
}