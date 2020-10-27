#pragma once

#include <thread>
#include "uv.h"
#include "ISerializable.h"
#include "CachedBuffer.h"
#include "PackageCodec.h"
#include "SerializableCache.h"

#define ERR_SUCCESS 0
#define ERR_NETWORK -1
#define ERR_NO_BUFFER -2

#define MAX_WRITE_IDLE_INTERVAL 10
#define MAX_READ_IDLE_INTERVAL 100

class SolcomSpi
{
private:
    /* data */
public:
    SolcomSpi(/* args */);
    virtual ~SolcomSpi();
    virtual void OnRecvMsg(ISerializable *recvMsg);
    virtual void OnDisconnect();
    virtual void OnConnect();
};

class SolcomApi
{
private:
    //spi
    SolcomSpi *solcomSpi;
    //发送缓存
    CachedBuffer<CByteArray> sendCache;
    //Ack发送管理<发送缓存,是否有Ack待发送>
    std::pair<CByteArray, bool> ackBuffer;
    //编解码器
    PackageCodec pkgCodec;
    //序列号
    uint64_t serialID = 0;
    //连接状态
    bool bConnected = false;
    //停止标志
    bool bStop = false;
    //是否自动重连
    bool bAutoReconn = true;
    //ISerializable对象缓存
    SerializableCache serializeObjCache;
    //接收字节缓存
    CByteArray recvBuffer;
    //接收状态
    ERecvStatType recvStat = ERecvStatType::Type_RecvHead;
    //下次需要接收字节数
    int nNeedRead = 0;
    //上次读成功时间
    int64_t lastRead;
    //上次写成功时间
    int64_t lastWrite;
    ///********************************libuv 相关************************************///
    uv_connect_t connHandle;       //connection handle
    uv_tcp_t tcpHandle;            //tcp handle
    uv_idle_t idleHandle;          //idle handle
    uv_timer_t recnTimerHandle;    //reconnec timer handle, trigger once
    uv_timer_t initTimerHandle;    //init timer handle, trigger once
    uv_loop_t loop;                //uv loop
    uv_buf_t req_uvbuf;            //uv send buf
    uv_write_t req_writereq;       //uv write req,uv_write_t 必须在uv_write的回调函数调用之后才能复用
    bool bReqWritereqBusy = false; //uv_write_t 忙标志
    uv_buf_t ack_uvbuf;            //uv send buf
    uv_write_t ack_writereq;       //uv write req,uv_write_t 必须在uv_write的回调函数调用之后才能复用
    bool bAckWritereqBusy = false; //uv_write_t 忙标志
    ///********************************libuv 相关************************************///
    //uvloop 工作线程
    std::thread loopThread;
    //服务端地址
    char servAddr[32];
    //服务端端口
    int servPort;
    typedef std::chrono::system_clock::time_point time_point;
    //loop开始时间
    time_point loopStartTime;
    //自动重连时间间隔（ms）
    int nMsReconInterval = 5000;
    //socket空闲时间间隔（ms）
    int nMsIdleInterval = 10000;

public:
    SolcomApi(/* args */);
    ~SolcomApi();
    //注册回调
    void RegistSpi(SolcomSpi *spi);
    //发送消息
    int PostMsg(ISerializable &msg);
    //注册服务端地址
    void RegistServer(char *pAddr, int nPort);
    //等待通信线程返回
    void Join();
    //开启API
    void Start();
    //关闭API
    void Stop();
    //是否自动重连
    void SetAutoReconnect(bool recon);

private:
    //获取loop开始至今的秒数
    int64_t getNowSeconds();
    //uvloop 工作线程
    void loopFunction();
    //关闭连接
    void closeConnection();
    //发起连接
    void connectServer();
    //发送心跳
    void sendHeartbeat();
    //发送应答
    void sendAck(PackageHeadField &pkgHead);
    //Req响应
    void reqHandle(CByteArray &inStream);
    //连接回调
    static void connect_cb(uv_connect_t *server, int status);
    //分配空间回调
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    //读回调
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    //Req写回调
    static void req_write_cb(uv_write_t *req, int status);
    //Ack写回调
    static void ack_write_cb(uv_write_t *req, int status);
    //重连定时器回调
    static void reconn_timer_cb(uv_timer_t *timer);
    //初始化定时器回调
    static void init_timer_cb(uv_timer_t *timer);
    //空闲回调
    static void idle_cb(uv_idle_t *handle);
};
