#pragma once

#include "uv.h"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <vector>
#include <queue>
#include <list>
#include <chrono>
#include "SolConnClient.h"
#include "ISerializable.h"
#include "PackageCodec.h"

class SolConnServer;
struct CommandTask
{
    uint8_t reqRtnType;
    int requestID;
    SolConnServer *server;
    SolConnClient *client;
    std::shared_ptr<ISerializable> cmd;
    CommandTask()
    {
        server = nullptr;
        client = nullptr;
    }
};

#define WPOLL_ADD 0
#define WPOLL_DEL 1

class SolConnServer
{
private:
    std::mutex mtxClients;
    std::unordered_map<std::string, SolConnClient *> mapConnClient;
    std::thread loopThread;
    int nPort = 60510;
    volatile bool bStop = false;

    //编解码器
    PackageCodec pkgCodec;

    typedef std::chrono::system_clock::time_point time_point;
    //loop开始时间
    time_point loopStartTime;
    /////客户端写注册列表////////
    std::list<SolConnClient *> listWevent;
    std::list<SolConnClient *> listWeventBck;
    std::mutex mtxWevent;
    ///////////////////////////

    /**********libuv 相关**********/
    uv_loop_t loop;
    uv_tcp_t tcpHandle; //监听socket
    uv_timer_t hbtTimerHandle;
    uv_idle_t idleHandle;
    /**********libuv 相关**********/

    //空闲检测时间间隔（ms）
    //hbtTimerHandle 触发时间间隔
    int nHbtChkInterval = 10000;

public:
    SolConnServer(/* args */);
    ~SolConnServer();
    void Start();
    void Stop();
    void Join();
    //客户端写注册/注销
    void WeventCtl(SolConnClient *client, int operate);

private:
    //uvloop线程函数
    void loopFunction();
    void writeHandle();
    //获取loop开始至今的秒数
    int64_t getNowSeconds();
    //关闭客户端
    void closeClient(SolConnClient *client);
    //获取客户端
    SolConnClient *getClientByKey(const std::string &strKey);
    static void connect_cb(uv_stream_t *server, int status);
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    static void req_write_cb(uv_write_t *req, int status);
    static void shutdown_cb(uv_shutdown_t *req, int status);
    static void hbt_timer_cb(uv_timer_t *timer);
    static void idle_cb(uv_idle_t *handle);
    //异步线程任务
    static void work_cb(uv_work_t *req);
    static void after_work_cb(uv_work_t *req, int status);

    friend class SolConnClient;
};
