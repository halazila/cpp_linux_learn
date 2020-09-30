#include <map>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "filedispatcher.h"

//客户端连接信息
class ClientConn
{
public:
    uv_tcp_t tcpHandle;     //tcp handle
    uv_idle_t idleHandle;   //idle handle
    uv_timer_t timerHandle; //timer handle
    std::string clientKey;  //格式，IPv4_addr:port，作为唯一连接标志
    int unreplyHeartbeat;   //未回复的心跳包个数
    int readBytes;          //已接收字节数
    // int pkgSize;                       //包大小
    int filesCapacity;                 //可同步的最大文件数
    int filesCount;                    //需要同步的文件数
    std::vector<FileInfo> uploadFiles; //需要同步的文件列表
    BytesLinkedBuf *readBuf;           //已接收数据缓存
    std::vector<PkgStruct> prsPkgs;    //解析的通信包
    PkgHead headCached;                //上一次解析的包头缓存
    bool bHeadCached;                  //是否有缓存包头
    bool bLogin;                       //是否登录
    bool bReadUpd;                     //是否读取新数据

public:
    ClientConn()
    {
        filesCapacity = DEFAULT_FILE_CAP;
        uploadFiles.reserve(filesCapacity);
        tcpHandle.data = idleHandle.data = timerHandle.data = this;
        unreplyHeartbeat = 0;
        readBytes = 0;
        filesCount = 0;
        bReadUpd = false;
        readBuf = nullptr;
        bLogin = bHeadCached = bReadUpd = false;
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
    //return -1,失败，其他；解析的通信包数目
    int parsePackage();
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
    static void connection_cb(uv_stream_t *server, int status);
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    static void write_cb(uv_write_t *req, int status);
    static void timer_cb(uv_timer_t *timer);
    static void shutdown_cb(uv_shutdown_t *req, int status);
    static void idle_cb(uv_idle_t *handle);
    int getClientKey(uv_tcp_t *client, std::string &strKey);

public:
    int addClientConn(ClientConn *conn);
    int removeClientConn(ClientConn *conn);
    int readProcess(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    void commandProcess(ClientConn *conn);
};
