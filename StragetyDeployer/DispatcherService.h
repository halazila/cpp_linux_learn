#pragma once

#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include "zmq.hpp"

using namespace std;

#define INPROC_BIND_ADDRESS "inproc://thread-message"

typedef chrono::system_clock SYSTEM_CLOCK;

//定时器
class Timer
{
private:
    SYSTEM_CLOCK::time_point trigtime;
    int nMills;
    function<void()> fHandle;

public:
    Timer()
    {
        nMills = 0;
        trigtime = SYSTEM_CLOCK::now();
    }
    Timer(int mills)
    {
        nMills = mills;
        trigtime = SYSTEM_CLOCK::now() + chrono::milliseconds(nMills);
    }
    void recount()
    {
        trigtime = SYSTEM_CLOCK::now() + chrono::milliseconds(nMills);
    }
    bool timeout(const SYSTEM_CLOCK::time_point &compares)
    {
        return trigtime <= compares;
    }
    void registHandle(function<void()> handle)
    {
        fHandle = handle;
    }
    void callback()
    {
        if (fHandle)
            return fHandle();
    }
    Timer &operator=(const Timer &other)
    {
        nMills = other.nMills;
        trigtime = other.trigtime;
        return *this;
    }
};

class DispatcherClient
{
private:
    string m_strRouter;   //ZMQ_ROUTER socket routerid
    string m_strIdentify; //user(manager)-id
    // 会话层 alive 探测
    SYSTEM_CLOCK::time_point m_tLastWrite;
    SYSTEM_CLOCK::time_point m_tLastRead;
    bool m_bLogin = false;

public:
    DispatcherClient();
    ~DispatcherClient();

    friend class DispatcherService;
};

class DispatcherService
{
private:
    zmq::context_t m_ctx;
    zmq::socket_t m_sockBind;
    zmq::socket_t m_sockInprocServer;
    // zmq::socket_t m_sockInprocClient;
    int m_nBindPort;
    unordered_map<string, shared_ptr<DispatcherClient>> m_mapClients; //<userid, client>
    // unordered_map<string, int> m_mapOnlineCns; //客户端多点同时在线统计
    //定时器+
    Timer m_timerKA;

public:
    DispatcherService();
    ~DispatcherService();
    void SetPort(int port) { m_nBindPort = port; }
    void StartService() { pollFunc(); }

private:
    void pollFunc();
    //内部线程发送
    void innerSendMsg(const char *data, int len, bool bMore = false);
    zmq::socket_t inProcSocket();
    //外部线程发送
    void outerSendMsg(zmq::socket_t &sndsocket, const char *data, int len, bool bMore = false);
    void katimeHandle();
    void onRecvCmd(int cmd, shared_ptr<DispatcherClient> &client);
    void onReqLogin(shared_ptr<DispatcherClient> &client); //bnew-是否新连接
    void onReqLogout(shared_ptr<DispatcherClient> &client);
    void onKeepAlive(shared_ptr<DispatcherClient> &client);
    bool identifyUser(const string &strid);
};
