#include "AsyncZmqApi.h"
#include "CommonStruct.h"

AsyncZmqApi::AsyncZmqApi()
{
    //context initial
    m_ctx = zmq::context_t(1);
    //tcp-socket initial
    m_sockTcp = zmq::socket_t(m_ctx, ZMQ_DEALER);
    //thread-socket server
    m_sockInprocServer = zmq::socket_t(m_ctx, ZMQ_ROUTER);
    m_sockInprocServer.bind(API_INPROC_BIND_ADDRESS);
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

void AsyncZmqApi::Start()
{
    m_bStop = false;
    if (!m_bPolling)
    {
        m_thdPoll = std::thread(std::bind(AsyncZmqApi::pollFunc, this));
    }
}

void AsyncZmqApi::Stop()
{
    m_bStop = true;
}

zmq::socket_t AsyncZmqApi::InProcSocket()
{
    zmq::socket_t socket(m_ctx, ZMQ_DEALER);
    socket.connect(API_INPROC_BIND_ADDRESS);
    return socket;
}

int AsyncZmqApi::Send(zmq::socket_t &socket, const char *data, int len, bool bLast)
{
    int ret = zmq_send(socket, data, len, bLast ? 0 : ZMQ_SNDMORE);
    if (ret == -1)
    {
        return ASYNCZMQAPI_ERROR;
    }
    return ASYNCZMQAPI_OK;
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
            //skip router-id
            m_sockInprocServer.recv(message);
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
            auto more_size = sizeof(more);
            while (1)
            {
                m_sockTcp.recv(message);
                m_sockTcp.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if (m_funcRecv)
                    m_funcRecv((char *)message.data(), message.size(), more ? false : true);
                if (!more)
                    break;
            }
        }
    }
    //disconnect tcp connection
    m_sockTcp.disconnect(m_strAddr);
    m_bInitConnect = false;

    m_bPolling = false;
}
