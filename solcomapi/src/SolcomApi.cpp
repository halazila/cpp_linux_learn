#include <functional>
#include <chrono>
#include "SolcomApi.h"
#include "SillyCommand.h"

//****SolcomSpi******/
SolcomSpi::SolcomSpi(/* args */)
{
}

SolcomSpi::~SolcomSpi()
{
}

void SolcomSpi::OnRecvMsg(ISerializable *recvMsg)
{
}

void SolcomSpi::OnDisconnect()
{
}

void SolcomSpi::OnConnect()
{
}

////****SolcomApi******////
SolcomApi::SolcomApi(/* args */)
    : sendCache(16), solcomSpi(nullptr)
{
    //忽略SIGPIPE信号
    signal(SIGPIPE, SIG_IGN);
    //
    uv_loop_init(&loop);
    uv_tcp_init(&loop, &tcpHandle);
    uv_idle_init(&loop, &idleHandle);
    uv_timer_init(&loop, &timerHandle);
    connHandle.data = this;
    tcpHandle.data = this;
    idleHandle.data = this;
    timerHandle.data = this;
    writereq.data = this;
    memset(servAddr, 0, sizeof(servAddr));
    servPort = 0;
}

SolcomApi::~SolcomApi()
{
}
//注册回调
void SolcomApi::RegistSpi(SolcomSpi *spi)
{
    solcomSpi = spi;
}
//发送消息
int SolcomApi::PostMsg(ISerializable &msg)
{
    if (!bConnected)
        return ERR_NETWORK;
    CByteArray *pByteArr = sendCache.allocate();
    if (pByteArr)
    {
        pByteArr->Init();
        PackageHeadField pkghead;
        memset(&pkghead, 0, sizeof(pkghead));
        pkghead.repeatTimes = 0;
        pkghead.reqAckType = EPkgReqAckType::Type_Req;
        pkghead.serialID = serialID++;
        //encode
        pByteArr->RightForward(sizeof(pkghead));
        pByteArr->LeftForward(sizeof(pkghead));
        pkgCodec.encodeSerializeObj(msg, *pByteArr);
        pkghead.pkgBodyLen = pByteArr->RightPos() - sizeof(pkghead);
        pByteArr->Prepend(pkghead);
        //缓存尾部指针前移
        sendCache.tailForward();
        return ERR_SUCCESS;
    }
    return ERR_NO_BUFFER;
}
//注册服务端地址
void SolcomApi::RegistServer(char *pAddr, int nPort)
{
    strcpy(servAddr, pAddr);
    servPort = nPort;
}
//等待通信线程返回
void SolcomApi::Join()
{
    if (loopThread.joinable())
    {
        loopThread.join();
    }
}
//开启API
void SolcomApi::Start()
{
    bStop = false;
    loopThread = std::thread(std::bind(&SolcomApi::loopFunction, this));
}
//关闭API
void SolcomApi::Stop()
{
    bStop = true;
}
//连接服务器
void SolcomApi::ConnectServer()
{
    ConnectCommand connCmd;
    PostMsg(connCmd);
}
//是否自动重连
void SolcomApi::SetAutoReconnect(bool recon)
{
    bAutoReconn = recon;
}

///private member function///
int64_t SolcomApi::getNowSeconds()
{
    auto tnow = std::chrono::system_clock::now();
    auto dura = std::chrono::duration_cast<std::chrono::seconds>(tnow - loopStartTime);
    return dura.count();
}

void SolcomApi::loopFunction()
{
    while (!bStop)
    {
        uv_idle_start(&idleHandle, &SolcomApi::idle_cb);
        loopStartTime = std::chrono::system_clock::now();
        lastRead = lastWrite = 0;
        uv_run(&loop, UV_RUN_DEFAULT);
        //close handles
        uv_close((uv_handle_t *)&tcpHandle, nullptr);
        uv_close((uv_handle_t *)&idleHandle, nullptr);
        uv_close((uv_handle_t *)&timerHandle, nullptr);
    }
}
//关闭连接
void SolcomApi::closeConnection()
{
    bConnected = false;
    uv_close((uv_handle_t *)&tcpHandle, nullptr);
    if (solcomSpi)
        solcomSpi->OnDisconnect();
}
//发起连接
void SolcomApi::connectServer()
{
    if (servAddr[0] == '\0')
    {
        return;
    }
    struct sockaddr_in dest;
    uv_ip4_addr(servAddr, servPort, &dest);
    int r = uv_tcp_connect(&connHandle, &tcpHandle, (const struct sockaddr *)&dest, &SolcomApi::connect_cb);
    if (r < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(r));
    }
}
//发送心跳
void SolcomApi::sendHeartbeat()
{
    HeartbeatCommand hbtCom;
    PostMsg(hbtCom);
}
//连接回调
void SolcomApi::connect_cb(uv_connect_t *server, int status)
{
    SolcomApi *pSolcom = (SolcomApi *)server->data;
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(status));
        if (pSolcom->solcomSpi)
            pSolcom->solcomSpi->OnDisconnect();
        if (pSolcom->bAutoReconn)
            uv_timer_start(&pSolcom->timerHandle, &SolcomApi::timer_cb, pSolcom->nMsReconInterval, 0);
    }
    else //连接成功
    {
        //置连接标志为true
        pSolcom->bConnected = true;
        //开启读监听
        uv_read_start((uv_stream_t *)&pSolcom->tcpHandle, &SolcomApi::alloc_cb, &SolcomApi::read_cb);
        if (pSolcom->solcomSpi)
            pSolcom->solcomSpi->OnConnect();
    }
}
//分配空间回调
void SolcomApi::alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    SolcomApi *pSolcom = (SolcomApi *)handle->data;
    if (pSolcom->recvStat == ERecvStatType::Type_RecvHead)
    {
        buf->len = PackageHeadLen;
        pSolcom->recvBuffer.Init();
        buf->base = pSolcom->recvBuffer.CharArray();
    }
    else
    {
        PackageHeadField pkghead = pSolcom->pkgCodec.decodeHead(pSolcom->recvBuffer);
        buf->len = pkghead.pkgBodyLen;
        buf->base = pSolcom->recvBuffer.CharArray() + pSolcom->recvBuffer.RightPos();
    }
}
//读回调
void SolcomApi::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    SolcomApi *pSolcom = (SolcomApi *)stream->data;
    //读取半包咋整呢？不考虑
    //TODO
}
//写回调
void SolcomApi::write_cb(uv_write_t *req, int status)
{
}
//定时器回调
void SolcomApi::timer_cb(uv_timer_t *timer)
{
}
//空闲回调
void SolcomApi::idle_cb(uv_idle_t *handle)
{
}