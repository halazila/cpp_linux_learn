#include <functional>
#include <chrono>
#include <iostream>
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
    switch (recvMsg->nType)
    {
    case ERequestType::TypeReq_Heartbeat:
        std::cout << "Receive heart beat" << std::endl;
        break;
    case ERequestType::TypeReq_TestCmd:
        ((TestCommand *)recvMsg)->Print();
        break;
    /**other command types**/
    default:
        break;
    }
}
void SolcomSpi::OnRspMsg(ISerializable *recvMsg, int requestID)
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
    : sendCache(128), solcomSpi(nullptr), ackBuffer(CByteArray(128), false)
{
    //忽略SIGPIPE信号
    signal(SIGPIPE, SIG_IGN);
    //
    uv_loop_init(&loop);
    uv_tcp_init(&loop, &tcpHandle);
    uv_idle_init(&loop, &idleHandle);
    uv_timer_init(&loop, &recnTimerHandle);
    uv_timer_init(&loop, &initTimerHandle);
    connHandle.data = this;
    tcpHandle.data = this;
    idleHandle.data = this;
    recnTimerHandle.data = this;
    req_writereq.data = this;
    ack_writereq.data = this;
    initTimerHandle.data = this;
    memset(servAddr, 0, sizeof(servAddr));
    servPort = 0;
}
//析构函数
SolcomApi::~SolcomApi()
{
    Join();
}
//注册回调
void SolcomApi::RegistSpi(SolcomSpi *spi)
{
    solcomSpi = spi;
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
//是否自动重连
void SolcomApi::SetAutoReconnect(bool recon)
{
    bAutoReconn = recon;
}
//发送请求
int SolcomApi::PostRequest(ISerializable &msg, int requestID)
{
    return postMsg(msg, requestID, EPkgReqRtnType::Type_Req);
}
//推送消息
int SolcomApi::PushMessage(ISerializable &msg)
{
    return postMsg(msg, 0, EPkgReqRtnType::Type_Rtn);
}

///******private member functions******///
int64_t SolcomApi::getNowSeconds()
{
    auto tnow = std::chrono::system_clock::now();
    auto dura = std::chrono::duration_cast<std::chrono::seconds>(tnow - loopStartTime);
    return dura.count();
}
//uvloop thread function
void SolcomApi::loopFunction()
{
    while (!bStop)
    {
        uv_idle_start(&idleHandle, &SolcomApi::idle_cb);
        uv_timer_start(&initTimerHandle, &SolcomApi::init_timer_cb, 100, 0);
        loopStartTime = std::chrono::system_clock::now();
        lastRead = lastWrite = 0;
        uv_run(&loop, UV_RUN_DEFAULT);
    }
}
//关闭连接
void SolcomApi::closeConnection()
{
    if (bConnected)
        uv_close((uv_handle_t *)&tcpHandle, nullptr);
    bConnected = false;
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
    postMsg(hbtCom);
}
//发送应答
void SolcomApi::sendAck(PackageHeadField &pkgHead)
{
    ackBuffer.first.Init();
    pkgCodec.encodeHead(pkgHead, ackBuffer.first);
    ackBuffer.second = true;
}
//Req响应
void SolcomApi::reqHandle(CByteArray &inStream)
{
    if (solcomSpi)
    {
        char *pchr = inStream.CharArray() + inStream.LeftPos();
        uint8_t objType = pchr[0];
        ISerializable *pCmdObj = serializeObjCache.GetCachedObj((ERequestType)objType);
        //反序列化
        pkgCodec.decodeSerializeObj(*pCmdObj, inStream);
        solcomSpi->OnRecvMsg(pCmdObj);
    }
}
int SolcomApi::postMsg(ISerializable &msg, int requestID, EPkgReqRtnType reqRtn)
{
    if (!bConnected)
        return ERR_NETWORK;
    CByteArray *pByteArr = sendCache.allocate();
    if (pByteArr)
    {
        pByteArr->Init();
        if (pByteArr->Capacity() < msg.dataSize() + PackageHeadLen)
            pByteArr->Reallocate(msg.dataSize() + PackageHeadLen);
        PackageHeadField pkghead;
        memset(&pkghead, 0, sizeof(pkghead));
        pkghead.reqRtnType = reqRtn;
        pkghead.requestID = requestID;
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
//连接回调
void SolcomApi::connect_cb(uv_connect_t *server, int status)
{
    SolcomApi *pSolcom = (SolcomApi *)server->data;
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(status));
        if (pSolcom->solcomSpi)
            pSolcom->solcomSpi->OnDisconnect();
        if (pSolcom->bAutoReconn && !pSolcom->bStop)
            uv_timer_start(&pSolcom->recnTimerHandle, &SolcomApi::reconn_timer_cb, pSolcom->nMsReconInterval, 0);
    }
    else //连接成功
    {
        //置连接标志为true
        pSolcom->bConnected = true;
        //开启读监听
        pSolcom->recvBuffer.Init();
        pSolcom->nNeedRead = PackageHeadLen;
        uv_read_start((uv_stream_t *)&pSolcom->tcpHandle, &SolcomApi::alloc_cb, &SolcomApi::read_cb);
        //开启idle
        uv_idle_start(&pSolcom->idleHandle, &SolcomApi::idle_cb);
        if (pSolcom->solcomSpi)
            pSolcom->solcomSpi->OnConnect();
    }
}
//分配空间回调
void SolcomApi::alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    SolcomApi *pSolcom = (SolcomApi *)handle->data;
    buf->len = pSolcom->nNeedRead;
    buf->base = pSolcom->recvBuffer.CharArray() + pSolcom->recvBuffer.RightPos();
}
//读回调
void SolcomApi::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    SolcomApi *pSolcom = (SolcomApi *)stream->data;
    if (nread > 0)
    {
        //缓存右指针右移
        pSolcom->recvBuffer.RightForward(nread);
        if (nread == pSolcom->nNeedRead) //接收到完整数据
        {
            PackageHeadField pkghead = pSolcom->pkgCodec.decodeHead(pSolcom->recvBuffer);
            if (pSolcom->recvStat == ERecvStatType::Type_RecvHead) //接收到完整的包头
            {
                //设置下次读取最大长度
                pSolcom->nNeedRead = pkghead.pkgBodyLen;
                pSolcom->recvStat = ERecvStatType::Type_RecvBody;
                //左指针归零
                pSolcom->recvBuffer.LeftBackward(PackageHeadLen);
                if (pSolcom->recvBuffer.Capacity() < pSolcom->nNeedRead + PackageHeadLen)
                    pSolcom->recvBuffer.Reallocate(pSolcom->nNeedRead + PackageHeadLen);
            }
            else //接收到完整的包体
            {
                //spi回调
                pSolcom->reqHandle(pSolcom->recvBuffer);
                //接收缓存状态初始化
                pSolcom->recvBuffer.Init();
                //接收状态改变
                pSolcom->recvStat = ERecvStatType::Type_RecvHead;
                pSolcom->nNeedRead = PackageHeadLen;
            }
        }
        else //接收到不完整数据
        {
            pSolcom->nNeedRead -= nread;
        }
    }
    else if (nread == 0)
    {
        //not error, EAGAIN or EWOULDBLOCK
    }
    else
    {
        //Error
        fprintf(stderr, "Read error: %s.\n", uv_strerror(nread));
        pSolcom->closeConnection();
        if (pSolcom->bAutoReconn && !pSolcom->bStop)
            uv_timer_start(&pSolcom->recnTimerHandle, &SolcomApi::reconn_timer_cb, pSolcom->nMsReconInterval, 0);
        return;
    }
    //更新上次读成功时间
    pSolcom->lastRead = pSolcom->getNowSeconds();
}
//Req写回调
void SolcomApi::req_write_cb(uv_write_t *req, int status)
{
    SolcomApi *pSolcom = (SolcomApi *)req->data;
    if (status < 0)
    {
        //Error
        fprintf(stderr, "Req Write error: %s.\n", uv_strerror(status));
        pSolcom->closeConnection();
        if (pSolcom->bAutoReconn && !pSolcom->bStop)
            uv_timer_start(&pSolcom->recnTimerHandle, &SolcomApi::reconn_timer_cb, pSolcom->nMsReconInterval, 0);
        return;
    }
    else
    {
        pSolcom->sendCache.headForward();
        pSolcom->bReqWritereqBusy = false;
    }
    //更新上次写成功时间
    pSolcom->lastWrite = pSolcom->getNowSeconds();
}
//重连定时器回调
void SolcomApi::reconn_timer_cb(uv_timer_t *timer)
{
    SolcomApi *pSolcom = (SolcomApi *)timer->data;
    //只处理重连
    if (!pSolcom->bConnected)
    {
        //tcp handle must be init with loop
        uv_tcp_init(&pSolcom->loop, &pSolcom->tcpHandle);
        pSolcom->tcpHandle.data = pSolcom;
        pSolcom->connectServer();
    }
}
//init定时器回调
void SolcomApi::init_timer_cb(uv_timer_t *timer)
{
    SolcomApi *pSolcom = (SolcomApi *)timer->data;
    //连接服务器
    pSolcom->connectServer();
}
//空闲回调
void SolcomApi::idle_cb(uv_idle_t *handle)
{
    SolcomApi *pSolcom = (SolcomApi *)handle->data;
    //idle should be stop or not, check with ::bStop
    bool bIdleStop = true;
    if (pSolcom->bConnected) //tcp处于连接状态
    {
        //write req
        if (!pSolcom->sendCache.isEmpty())
        {
            bIdleStop = false;
            if (!pSolcom->bReqWritereqBusy)
            {
                CByteArray *byteArr = pSolcom->sendCache.current();
                if (nullptr != byteArr)
                {
                    pSolcom->req_uvbuf = uv_buf_init(byteArr->CharArray(), byteArr->RightPos());
                    pSolcom->bReqWritereqBusy = true;
                    uv_write(&pSolcom->req_writereq, (uv_stream_t *)&pSolcom->tcpHandle, &pSolcom->req_uvbuf, 1, &SolcomApi::req_write_cb);
                }
            }
        }
        if (!pSolcom->bStop)
        {
            int64_t tnow = pSolcom->getNowSeconds();
            //keep-alive检测
            int64_t writeInterval = tnow - pSolcom->lastWrite;
            if (writeInterval > MAX_WRITE_IDLE_INTERVAL)
            {
                //客户端主动发送心跳，心跳无论发送成功与否都更新lastWrite
                pSolcom->sendHeartbeat();
                pSolcom->lastWrite = tnow;
            }
            //重连检测
            int64_t readInterval = tnow - pSolcom->lastWrite;
            if (readInterval > MAX_IDLE_INTERVAL)
            {
                //重连
                pSolcom->closeConnection();
                if (pSolcom->bAutoReconn && !pSolcom->bStop)
                    pSolcom->connectServer();
            }
        }
    }

    //停止idle
    if (pSolcom->bStop && bIdleStop)
    {
        uv_idle_stop(handle);
        pSolcom->closeConnection();
    }
}