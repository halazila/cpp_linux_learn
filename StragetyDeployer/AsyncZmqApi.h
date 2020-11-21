#include <thread>
#include <functional>
#include "zmq.hpp"

#define RecvFunction std::function<void(char *data, int len, bool bLast)>

#define ASYNCZMQAPI_OK 0
#define ASYNCZMQAPI_ERROR -1

class AsyncZmqApi;
class TcpSockConnectMonitor : public zmq::monitor_t
{
private:
    AsyncZmqApi *m_pAsyncApi = nullptr;

public:
    TcpSockConnectMonitor() {}
    ~TcpSockConnectMonitor() {}
    void on_event_connected(const zmq_event_t &event_, const char *addr_) override
    {
        if (m_pAsyncApi)
            m_pAsyncApi->OnConnected();
    }
    void on_event_disconnected(const zmq_event_t &event_, const char *addr_) override
    {
        if (m_pAsyncApi)
            m_pAsyncApi->OnDisconnected();
    }
}

class AsyncZmqApi
{
private:
    zmq::context_t m_ctx;                 //上下文
    zmq::socket_t m_sockTcp;              //tcp通信套接字
    zmq::socket_t m_sockInprocServer;     //线程通信套接字-server
    zmq::socket_t m_sockInprocClient;     //线程通信套接字-client
    TcpSockConnectMonitor m_monitConnect; //tcp连接监视器
    std::thread m_thdPoll;                //poll 线程
    bool m_bInitConnect = false;          //tcp通信套接字是否连接
    volatile bool m_bStop = false;
    volatile bool m_bPolling = false;
    volatile bool m_bTcpConnected = false;
    RecvFunction m_funcRecv;   //消息回调函数
    std::string m_strIdentify; //连接用户名
    std::string m_strAddr;     //服务端地址

public:
    AsyncZmqApi();
    ~AsyncZmqApi();
    //连接状态
    bool IsConnected()
    {
        return m_bTcpConnected;
    }
    //设置远端地址,格式 tcp://127.0.0.1:5889
    void SetRemoteAddr(const std::string &strAddr)
    {
        m_strAddr = strAddr;
    }
    void SetIdentify(const std::string &strIdentify)
    {
        m_strIdentify = strIdentify;
    }
    //设置接收回调
    void SetRecvCallback(RecvFunction recvFunc)
    {
        m_funcRecv = recvFunc;
    }
    //连接远端服务器
    int Connect();
    void Stop();
    void Start();
    //发送数据
    int Send(char *data, int len, bool bLast = true);
    virtual void OnConnected();
    virtual void OnDisconnected();

private:
    void pollFunc();
};