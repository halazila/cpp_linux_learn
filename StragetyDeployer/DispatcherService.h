#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <mutex>
#include "zmq.hpp"
#include "SQLiteDatabase.h"
#include "FixedThreaPool.h"
#include "dbopmacros.h"
#include "dboperatefunc.h"
#include "CommonStruct.h"

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
    FixedThreaPool threadPool;
    unordered_set<string> m_setManagerCache;
    //对ManageUser表修改操作标志
    atomic_bool m_bUpdManagerCache;
    mutex m_mtxSqlite; //sqlite 锁

public:
    DispatcherService();
    ~DispatcherService();
    void SetPort(int port) { m_nBindPort = port; }
    void StartService()
    {
        char addr[128] = {0};
        sprintf(addr, "tcp://*:%d", m_nBindPort);
        m_sockBind.bind(addr);
        pollFunc();
    }

private:
    void updManagerCache();
    void pollFunc();
    //内部线程发送
    void innerSendMsg(void *data, int len, bool bMore = false);
    zmq::socket_t inProcSocket();
    //外部线程发送
    void outerSendMsg(zmq::socket_t &sndsocket, void *data, int len, bool bMore = false);
    void katimeHandle();
    void onRecvCmd(int cmd, shared_ptr<DispatcherClient> client);
    void onReqLogin(shared_ptr<DispatcherClient> client);
    void onReqLogout(shared_ptr<DispatcherClient> client);
    void onKeepAlive(shared_ptr<DispatcherClient> client);
    bool identifyUser(const string &strid);
    void skipRemainMFrame(); //忽略剩余的消息帧
    template <class T>       //接收单一平凡对象
    void recvPodObject(zmq::message_t &message, T &dataRef);
    template <class T> //接收对象数组
    void recvVecObject(zmq::message_t &message, vector<T> &objVec);
    /////////数据库操作响应///////
    template <class T>
    void onQryBySql(shared_ptr<DispatcherClient> client, const string &sql, int eletype, int requestId);
    template <class T>
    void onInsByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId);
    template <class T>
    void onDelByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId);
    template <class T>
    void onUpdByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId);
    template <class T>
    void storeCacheBool(bool b);
};

template <class T> //接收单一平凡对象
void DispatcherService::recvPodObject(zmq::message_t &message, T &dataRef)
{
    m_sockBind.recv(message);
    assert(message.size() == sizeof(T));
    memcpy(&dataRef, (char *)message.data(), sizeof(T));
}
template <class T> //接收对象数组
void DispatcherService::recvVecObject(zmq::message_t &message, vector<T> &objVec)
{
    int more = 0;
    auto more_size = sizeof(more);
    T tmp;
    while (1)
    {
        m_sockBind.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        if (!more)
            break;
        m_sockBind.recv(message);
        memcpy(&tmp, message.data(), sizeof(T));
        objVec.push_back(tmp);
    }
}
/////////数据库操作响应///////
template <class T>
void DispatcherService::onQryBySql(shared_ptr<DispatcherClient> client, const string &sql, int eletype, int requestId)
{
    unique_lock<decltype(m_mtxSqlite)> sqliteLock(m_mtxSqlite);
    vector<T> qryObjs = qryDataBySql<T>(sql);
    sqliteLock.unlock();
    zmq::socket_t socket = inProcSocket();
    //routerid
    outerSendMsg(socket, (void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    //passiveResponse
    int stype = STCMsgPattern::TPassiveResponse;
    outerSendMsg(socket, &stype, sizeof(int), true);
    //response
    ReqResponse rsp;
    rsp.RequestID = requestId;
    rsp.CmdType = ECommandType::TQuery;
    rsp.ErrorID = EResponseErrType::TSuccess;
    strcpy(rsp.ErrMsg, "Query Successfully");
    outerSendMsg(socket, &rsp, sizeof(rsp), qryObjs.size() > 0);
    //element type
    outerSendMsg(socket, &eletype, sizeof(eletype), qryObjs.size() > 0);
    //elements
    for (int i = 0; i < qryObjs.size(); i++)
    {
        outerSendMsg(socket, &qryObjs[i], sizeof(T), i < qryObjs.size() - 1);
    }
}
template <class T>
void DispatcherService::onInsByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId)
{
    unique_lock<decltype(m_mtxSqlite)> sqliteLock(m_mtxSqlite);
    int res = insByObjects<T>(objVec);
    sqliteLock.unlock();
    zmq::socket_t socket = inProcSocket();
    outerSendMsg(socket, (void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int stype = STCMsgPattern::TPassiveResponse;
    outerSendMsg(socket, &stype, sizeof(int), true);
    ReqResponse rsp;
    rsp.RequestID = requestId;
    rsp.CmdType = ECommandType::TInsert;
    if (res < 0)
    {
        rsp.ErrorID = EResponseErrType::TDbError;
        strcpy(rsp.ErrMsg, "Insert Failed");
    }
    else
    {
        rsp.ErrorID = EResponseErrType::TSuccess;
        strcpy(rsp.ErrMsg, "Insert Successfully");
        storeCacheBool<T>(true);
    }
    outerSendMsg(socket, &rsp, sizeof(rsp));
}
template <class T>
void DispatcherService::onDelByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId)
{
    unique_lock<decltype(m_mtxSqlite)> sqliteLock(m_mtxSqlite);
    int res = delByObjects<T>(objVec);
    sqliteLock.unlock();
    zmq::socket_t socket = inProcSocket();
    outerSendMsg(socket, (void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int stype = STCMsgPattern::TPassiveResponse;
    outerSendMsg(socket, &stype, sizeof(int), true);
    ReqResponse rsp;
    rsp.RequestID = requestId;
    rsp.CmdType = ECommandType::TDelete;
    if (res < 0)
    {
        rsp.ErrorID = EResponseErrType::TDbError;
        strcpy(rsp.ErrMsg, "Delete Failed");
    }
    else
    {
        rsp.ErrorID = EResponseErrType::TSuccess;
        strcpy(rsp.ErrMsg, "Delete Successfully");
        storeCacheBool<T>(true);
    }
    outerSendMsg(socket, &rsp, sizeof(rsp));
}
template <class T>
void DispatcherService::onUpdByObjects(shared_ptr<DispatcherClient> client, const vector<T> &objVec, int requestId)
{
    unique_lock<decltype(m_mtxSqlite)> sqliteLock(m_mtxSqlite);
    int res = updByObjects<T>(objVec);
    sqliteLock.unlock();
    zmq::socket_t socket = inProcSocket();
    outerSendMsg(socket, (void *)client->m_strRouter.c_str(), client->m_strRouter.length(), true);
    int stype = STCMsgPattern::TPassiveResponse;
    outerSendMsg(socket, &stype, sizeof(int), true);
    ReqResponse rsp;
    rsp.RequestID = requestId;
    rsp.CmdType = ECommandType::TUpdate;
    if (res < 0)
    {
        rsp.ErrorID = EResponseErrType::TDbError;
        strcpy(rsp.ErrMsg, "Update Failed");
    }
    else
    {
        rsp.ErrorID = EResponseErrType::TSuccess;
        strcpy(rsp.ErrMsg, "Update Successfully");
        storeCacheBool<T>(true);
    }
    outerSendMsg(socket, &rsp, sizeof(rsp));
}

template <class T>
inline void DispatcherService::storeCacheBool(bool b)
{
}

template <>
inline void DispatcherService::storeCacheBool<ManageUser>(bool b)
{
    m_bUpdManagerCache.store(true);
}