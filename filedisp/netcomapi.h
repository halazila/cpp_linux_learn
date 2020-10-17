#include <queue>
#include <mutex>
#include <thread>
#include "uv.h"
#include "pkgcodec.h"

//任务队列，用于缓存异步任务
template <class T>
class TaskQueue
{
private:
    std::mutex queMutex;
    std::queue<T *> taskQueue;

public:
    TaskQueue(){};
    ~TaskQueue() { clear(); };
    void push(T *pkgStream)
    {
        std::lock_guard<decltype(queMutex)> lock(queMutex);
        taskQueue.push(pkgStream);
    };
    T *pop()
    {
        std::lock_guard<decltype(queMutex)> lock(queMutex);
        T *ret = nullptr;
        if (!taskQueue.empty())
        {
            ret = taskQueue.front();
            taskQueue.pop();
        }
        return ret;
    };
    T *front()
    {
        if (!taskQueue.empty())
        {
            return taskQueue.front();
        }
        return nullptr;
    };
    bool empty()
    {
        return taskQueue.empty();
    };
    void clear()
    {
        T *p = nullptr;
        while (!taskQueue.empty())
        {
            p = taskQueue.front();
            delete p;
            taskQueue.pop();
        }
    };
};

//NetcomApi回调类
class NetcomHandler
{
public:
    NetcomHandler(){};
    virtual ~NetcomHandler(){};
    virtual void pkgStreamHandle(PkgStreamInterface *pkgStream) = 0;
};

//通信层封装，基于libuv
class NetcomApi
{
private:
    uv_connect_t connHandle; //connection handle
    uv_tcp_t tcpHandle;      //tcp handle
    uv_idle_t idleHandle;    //idle handle
    uv_timer_t timerHandle;  //timer handle
    uv_loop_t loop;          //uv loop
    uv_buf_t senduvbuf;      //uv send buf
    uv_write_t writereq;     //uv write req
    BytesBuffer recvBuffer;  //接收buffer
    BytesBuffer sendBuffer;  //发送buffer
    typedef TaskQueue<PkgStreamInterface> NetTaskQueue;
    NetTaskQueue sendTaskQue; //发送任务队列
    int64_t lastRead;         //上次读成功的时间
    int64_t lastWrite;        //上次写成功的时间
    std::thread loopThread;   //loop 工作线程
    PkgCodec pkgCodec;
    bool bStop = false;
    bool bConnected = false;
    char servAddr[32];
    int servPort;
    NetcomHandler *pNetcomHandler;

    typedef std::chrono::system_clock::time_point time_point;
    time_point loopStartTime; //loop开始时间

public:
    NetcomApi();
    ~NetcomApi();
    void postPackage(PkgStreamInterface *pkgSteam);
    void registServer(char *pAddr, int nPort);
    void join();
    NetcomHandler *getHandler();
    void registerHandler(NetcomHandler *handler);
    void startLoop();
    void stopLoop();
    int64_t getNowSeconds(); //计算loop开始至今的秒数

private:
    static void connect_cb(uv_connect_t *server, int status);
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    static void write_cb(uv_write_t *req, int status);
    static void timer_cb(uv_timer_t *timer);
    static void idle_cb(uv_idle_t *handle);
    void loopFunction();
    void closeConnection();
    void connectServer();
    void sendHeartbeat();
};
