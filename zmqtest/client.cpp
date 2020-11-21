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
        int nsend = 0;
        nsend = zmq_send(requester, "", 0, ZMQ_SNDMORE);
        memset(sendstr, 0, sizeof(sendstr));
        sprintf(sendstr, "Hello_%d", i);
        nsend = zmq_send(requester, sendstr, strlen(sendstr), 0);
        std::cout << "send data: " << sendstr << std::endl;

        // zmq::pollitem_t items[] = {
        // {requester, 0, ZMQ_POLLOUT, 0}};
        // int npoll = zmq::poll(items, 1, 1000);
        // if (npoll==0)
        // {
        //     std::cout<<"zmq::poll timeout"<<std::endl;
        //     return 0;
        // }else if(npoll<0)
        // {
        //     std::cout<<"zmq::poll error"<<std::endl;
        //     return -1;
        // }
        // items[0] = {requester, 0, ZMQ_POLLIN, 0};
        // npoll = zmq::poll(items, 1, 1000);
        // if (npoll==0)
        // {
        //     std::cout<<"zmq::poll timeout"<<std::endl;
        //     return 0;
        // }else if(npoll<0)
        // {
        //     std::cout<<"zmq::poll error"<<std::endl;
        //     return -1;
        // }

        ///*******///
        // int nrecv;
        // nrecv = zmq_recv(requester, recvstr, 31, 0); //zmq_recv
        // assert(nrecv == 0);
        // nrecv = zmq_recv(requester, recvstr, 31, 0); //zmq_recv
        // if (nrecv > 31)
        // {
        //     recvstr[31] = '\0';
        // }
        ///*****///
        requester.recv(message); //message_t recv
        assert(message.size() == 0);
        requester.recv(message); //message_t recv
        memcpy(recvstr, message.data(), message.size());
        recvstr[message.size()] = '\0';
        std::cout << "recv data: " << recvstr << std::endl;
    }

    return 0;
}