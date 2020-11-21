#include "AsyncZmqApi.h"

AsyncZmqApi::AsyncZmqApi()
{
    //context initial
    m_ctx = zmq::context_t(1);
    //tcp-socket initial
    m_sockTcp = zmq::socket_t(m_ctx, ZMQ_DEALER);
    //thread-socket server
    m_sockInprocServer = zmq::socket_t(m_ctx, ZMQ_PAIR);
    m_sockInprocServer.bind("inproc://thread-message");
    //thread-socket client
    m_sockInprocClient = zmq::socket_t(m_ctx, ZMQ_PAIR);
    m_sockInprocClient.connect("inproc://thread-message");
    //monitor initialization
    m_monitConnect.init(m_sockTcp, "inproc://monitor-tcpclient", ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
}

AsyncZmqApi::~AsyncZmqApi()
{
    if (m_thdPoll.joinable())
        m_thdPoll.join();
}

int AsyncZmqApi::Connect()
{
    if (!m_bInitConnect)
    {
        if (!m_strAddr.empty())
        {
            m_sockTcp.connect(m_strAddr);
            m_bInitConnect = true;
        }
        else
        {
            return ASYNCZMQAPI_ERROR;
        }
    }
    return ASYNCZMQAPI_OK;
}

void AsyncZmqApi::Stop()
{
    m_bStop = true;
    if (m_thdPoll.joinable())
        m_thdPoll.join();
}

void AsyncZmqApi::Start()
{
    m_bStop = false;
    if (!m_bPolling)
    {
        m_thdPoll = std::thread(std::bind(AsyncZmqApi::pollFunc, this));
    }
}

int AsyncZmqApi::Send(char *data, int len, bool bLast)
{
    zmq_send(m_sockInprocClient, data, len, bLast ? 0 : ZMQ_SNDMORE);
}

void AsyncZmqApi::pollFunc()
{
    if (!m_bStop)
        m_bPolling = true;
    while (!m_bStop)
    {
        m_monitConnect.check_event(0);
        ///poll
        zmq::pollitem_t items[] =
            {
                {m_sockInprocServer, 0, ZMQ_POLLIN, 0},
                {m_sockTcp, 0, ZMQ_POLLIN, 0},
            };
        zmq::message_t message;
        zmq::poll(items, 2, 0);
        if (items[0].revents & ZMQ_POLLIN)
        {
            int more;
            while (1)
            {
                m_sockInprocServer.recv(message);
                auto more_size = sizeof(more);
                m_sockInprocServer.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                m_sockTcp.send(message, more ? ZMQ_SNDMORE : 0);
                if (!more)
                    break;
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            int more;
            while (1)
            {
                m_sockTcp.recv(message);
                auto more_size = sizeof(more);
                m_sockTcp.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if (m_funcRecv)
                    m_funcRecv(message.data(), message.size(), more ? false : true);
                if (!more)
                    break;
            }
        }
    }
    m_bPolling = false;
}

void AsyncZmqApi::OnConnected()
{
    m_bTcpConnected = true;
    Send(m_strIdentify.c_str(), m_strIdentify.length());
}

void AsyncZmqApi::OnDisconnected()
{
    m_bTcpConnected = false;
}
