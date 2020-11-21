#include "DispatcherService.h"

DispatcherService::DispatcherService(/* args */)
{
    m_ctx = zmq::context(1);
    m_sockBind = zmq::socket_t(m_ctx, ZMQ_ROUTER);
    //thread-socket server
    m_sockInprocServer = zmq::socket_t(m_ctx, ZMQ_PAIR);
    m_sockInprocServer.bind("inproc://thread-message");
    //thread-socket client
    m_sockInprocClient = zmq::socket_t(m_ctx, ZMQ_PAIR);
    m_sockInprocClient.connect("inproc://thread-message");
}

DispatcherService::~DispatcherService()
{
}

void DispatcherService::pollFunc()
{
    while (1)
    {
        zmq::pollitem_t items[] =
            {
                {m_sockInprocServer, 0, ZMQ_POLLIN, 0},
                {m_sockBind, 0, ZMQ_POLLIN, 0},
            };
        zmq::message_t message;
        zmq::poll(items, 2, 1000);
        if (items[0].revents & ZMQ_POLLIN)
        {
            int more;
            while (1)
            {
                m_sockInprocServer.recv(message);
                auto more_size = sizeof(more);
                m_sockInprocServer.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                m_sockBind.send(message, more ? ZMQ_SNDMORE : 0);
                if (!more)
                    break;
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            //todo
            //router id
            m_sockBind.recv(message);
            std::string strRouter((char*)message.data(),message.size());
            
        }
    }
}

void DispatcherService::sendMsg(char *data, int len, bool bMore)
{
    zmq_send(m_sockInprocClient, data, len, bMore ? ZMQ_SNDMORE : 0);
}
