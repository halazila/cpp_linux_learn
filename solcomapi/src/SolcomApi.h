#pragma once

#include <thread>
#include "uv.h"
#include "ISerializable.h"
#include "CachedBuffer.h"
#include "PackageCodec.h"

#define ERR_SUCCESS 0
#define ERR_NETWORK -1
#define ERR_NO_BUFFER -2

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
    SolcomSpi *solcomSpi;
    //发送缓存
    CachedBuffer<CByteArray> sendCache;
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
    //接收字节缓存
    CByteArray recvBuffer;
    //接收状态
    ERecvStatType recvStat = ERecvStatType::Type_RecvHead;
    //上次读成功时间
    int64_t lastRead;
    //上次写成功时间
    int64_t lastWrite;
    ///*****libuv 相关*****///
    uv_connect_t connHandle; //connection handle
    uv_tcp_t tcpHandle;      //tcp handle
    uv_idle_t idleHandle;    //idle handle
    uv_timer_t timerHandle;  //timer handle
    uv_loop_t loop;          //uv loop
    uv_buf_t senduvbuf;      //uv send buf
    uv_write_t writereq;     //uv write req
    ///*****libuv 相关*****///
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
    //连接服务器
    void ConnectServer();
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
    //连接回调
    static void connect_cb(uv_connect_t *server, int status);
    //分配空间回调
    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    //读回调
    static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    //写回调
    static void write_cb(uv_write_t *req, int status);
    //定时器回调
    static void timer_cb(uv_timer_t *timer);
    //空闲回调
    static void idle_cb(uv_idle_t *handle);
};
