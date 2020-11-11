#include "zmq.hpp"
#include <iostream>
#include <thread>

int main()
{
    zmq::context_t context(1);
    // zmq::socket_t requester(context, ZMQ_REQ);
    zmq::socket_t requester(context, ZMQ_DEALER);
    requester.connect("tcp://127.0.0.1:5559");

    char sendstr[32] = {0};
    char recvstr[32] = {0};
    zmq::message_t message(32);

    auto version = zmq::version();
    std::cout << std::get<0>(version) << "." << std::get<1>(version) << "." << std::get<2>(version) << std::endl;

    for (int i = 0; i < 10; i++)
    {
        ///////////////////req socket////////////////////
        // memset(sendstr, 0, sizeof(sendstr));
        // sprintf(sendstr, "HawaiHello_%d", i);
        // zmq_send(requester, sendstr, strlen(sendstr), 0);
        // std::cout << "send data: " << sendstr << std::endl;
        // memset(recvstr, 0, sizeof(recvstr));
        // int nrecv = zmq_recv(requester, recvstr, 32, 0);
        // if (nrecv > 31)
        // {
        //     recvstr[31] = '\0';
        // }
        // std::cout << "recv data: " << recvstr << std::endl;

        ///////////////////dealer socket//////////////////
        zmq_send(requester, "", 0, ZMQ_SNDMORE);
        memset(sendstr, 0, sizeof(sendstr));
        sprintf(sendstr, "Hello_%d", i);
        zmq_send(requester, sendstr, strlen(sendstr), 0);
        std::cout << "send data: " << sendstr << std::endl;
        int nrecv;
        nrecv = zmq_recv(requester, recvstr, 31, 0); //zmq_recv
        assert(nrecv == 0);
        nrecv = zmq_recv(requester, recvstr, 31, 0); //zmq_recv
        if (nrecv > 31)
        {
            recvstr[31] = '\0';
        }
        // requester.recv(message); //message_t recv
        // requester.recv(message); //message_t recv
        // memcpy(recvstr, message.data(), message.size());
        std::cout << "recv data: " << recvstr << std::endl;
    }

    return 0;
}