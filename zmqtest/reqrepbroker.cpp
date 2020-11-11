#include "zmq.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t frontend(context, ZMQ_ROUTER);
    zmq::socket_t backend(context, ZMQ_DEALER);

    frontend.bind("tcp://*:5559");
    backend.bind("tcp://*:5560");

    zmq::pollitem_t items[] = {
        {frontend, 0, ZMQ_POLLIN, 0},
        {backend, 0, ZMQ_POLLIN, 0}};

    while (1)
    {
        zmq::message_t message;
        int more;
        zmq::poll(items, 2, -1);
        if (items[0].revents & ZMQ_POLLIN)
        {
            while (1)
            {
                frontend.recv(message);
                auto more_size = sizeof(more);
                frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                backend.send(message, more ? ZMQ_SNDMORE : 0);
                if (!more)
                    break;
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            while (1)
            {
                backend.recv(message);
                auto more_size = sizeof(more);
                backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                frontend.send(message, more ? ZMQ_SNDMORE : 0);
                if (!more)
                    break;
            }
        }
    }

    return 0;
}