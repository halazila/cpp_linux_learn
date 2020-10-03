#include <map>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "filedispatcher.h"

//客户端连接信息
class ClientConn
{
public:
    uv_tcp_t tcpHandle;          //tcp handle
    uv_idle_t idleHandle;        //idle handle
    uv_timer_t timerHandle;      //timer handle
    std::string clientKey;       //格式，IPv4_addr:port，作为唯一连接标志
    int unreplyHeartbeat;        //未回复的心跳包个数
    BytesLinkedBuf *readBuf;     //原始接收数据缓存，用于在idle-handle callback 做包解析
    RecvStatusField recvStatus;  //当前接收状态，用于包解析
    CommondStructField cmdField; //当前接收的命令，用于命令处理
    bool bLogin;                 //是否登录
    int fd;                      //关联文件描述符

public:
    ClientConn()
    {
        tcpHandle.data = idleHandle.data = timerHandle.data = this;
        unreplyHeartbeat = 0;
        readBuf = nullptr;
        bLogin = false;
        fd = -1;
    }
    ~ClientConn()
    {
        if (readBuf)
        {
            delete readBuf;
        }
    }
    //init所有handle
    void initHandle(uv_loop_t *loop);
    //close所有handle
    void closeHandle();
    //缓存收到的字节
    void appendRecvBytes(const char *pChr, const int len);
    //return -1-失败; 0-无可解析数据; 1-只解析部分数据; 2-解析到完整包
    int packageHandle();
};

//服务端
class DispatchServer
{
private:
    static DispatchServer *pGlobalInstance;
    static std::mutex globalMtx;
    uv_loop_t *loop;
    std::mutex m_mtxMapClientConns;
    std::unordered_map<std::string, ClientConn *> m_mapClientConns; //key, IP4_Addr:port

private:
    DispatchServer(/* args */);
    ~DispatchServer();

public:
    static DispatchServer *getInstance();
    static void delInstance();

private:
    //tcp-handle connect callback
    static void connection_cb(uv_stream_t *server, int status);
    //tcp-handle read allocation callback
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    //tcp-handle read callback
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    //tcp-handle write callback
    static void write_cb(uv_write_t *req, int status);
    //tcp-handle shutdown callback
    static void shutdown_cb(uv_shutdown_t *req, int status);
    //timer-handle countdown callback
    static void timer_cb(uv_timer_t *timer);
    //idle-handle callback
    static void idle_cb(uv_idle_t *handle);
    int getClientKey(uv_tcp_t *client, std::string &strKey);

public:
    int addClientConn(ClientConn *conn);
    int removeClientConn(ClientConn *conn);
    int readProcess(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    void commandProcess(ClientConn *conn);
};
