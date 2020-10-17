#include <signal.h>
#include "netcomapi.h"

/////////////////////NetcomApi//////////////////////////////////
NetcomApi::NetcomApi()
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
NetcomApi::~NetcomApi()
{
    if (loopThread.joinable())
        loopThread.join();
}
void NetcomApi::postPackage(PkgStreamInterface *pkgSteam)
{
    sendTaskQue.push(pkgSteam);
}
void NetcomApi::registServer(char *pAddr, int nPort)
{
    strcpy(servAddr, pAddr);
    servPort = nPort;
}
void NetcomApi::join()
{
    if (loopThread.joinable())
    {
        loopThread.join();
    }
}
NetcomHandler *NetcomApi::getHandler()
{
    return pNetcomHandler;
}
void NetcomApi::registerHandler(NetcomHandler *handler)
{
    pNetcomHandler = handler;
}
void NetcomApi::startLoop()
{
    bStop = false;
    loopThread = std::thread(&NetcomApi::loopFunction);
}
void NetcomApi::stopLoop()
{
    bStop = true;
}
int64_t NetcomApi::getNowSeconds()
{
    auto tnow = std::chrono::system_clock::now();
    auto dura = std::chrono::duration_cast<std::chrono::seconds>(tnow - loopStartTime);
    return dura.count();
}

/**libuv回调函数**/
void NetcomApi::connect_cb(uv_connect_t *server, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(status));
    }
    else //连接成功
    {
        NetcomApi *pNetcom = (NetcomApi *)server->data;
        //置连接标志为true
        pNetcom->bConnected = true;
        //开启读监听
        uv_read_start((uv_stream_t *)&pNetcom->tcpHandle, &NetcomApi::alloc_cb, &NetcomApi::read_cb);
    }
}
void NetcomApi::alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    NetcomApi *pNetcom = (NetcomApi *)handle->data;
    buf->len = pNetcom->recvBuffer.totalSize;
    buf->base = pNetcom->recvBuffer.byteBuffer;
}
void NetcomApi::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
}
void NetcomApi::write_cb(uv_write_t *req, int status)
{
    NetcomApi *pNetcom = (NetcomApi *)req->data;
    //无论写是否成功，都更新lastWrite
    pNetcom->lastWrite = pNetcom->getNowSeconds();
    pNetcom->recvBuffer.clear();
    if (status < 0)
    {
        fprintf(stderr, "Write error: %s.\n", uv_strerror(status));
    }
    else
    {
    }
}
void NetcomApi::timer_cb(uv_timer_t *timer)
{
}
void NetcomApi::idle_cb(uv_idle_t *handle)
{
    NetcomApi *pNetcom = (NetcomApi *)handle->data;
    if (pNetcom->bStop)
    {
        uv_idle_stop(&pNetcom->idleHandle);
    }
    else
    {
        //处于连接状态
        if (pNetcom->bConnected)
        {
            //发送缓存的请求
            if (!pNetcom->sendBuffer.isBusy && !pNetcom->sendTaskQue.empty())
            {
                PkgStreamInterface *pStream = pNetcom->sendTaskQue.pop();
                pNetcom->sendBuffer.isBusy = true;
                pNetcom->pkgCodec.encode(&pNetcom->sendBuffer, pStream);
                pNetcom->senduvbuf = uv_buf_init(pNetcom->sendBuffer.byteBuffer, pNetcom->sendBuffer.usedSize);
                uv_write(&pNetcom->writereq, (uv_stream_t *)&pNetcom->tcpHandle, &pNetcom->senduvbuf, 1, &NetcomApi::write_cb);
                delete pStream;
            }
            //空闲检测
            int64_t tnow = pNetcom->getNowSeconds();
            int64_t interval = tnow - pNetcom->lastRead;
            if (interval < MAX_IDLE_INTERVAL)
            {
                //nothing todo
            }
            else if (interval < MAX_TIMEOUT_INTERVAL)
            {
                //发送心跳探测
                pNetcom->sendHeartbeat();
            }
            else
            {
                //断开连接
                pNetcom->closeConnection();
            }
        }
        else
        {
            //断线状态休眠2s后重连server
            std::this_thread::sleep_for(std::chrono::duration<int>(2));
            pNetcom->connectServer();
        }
    }
}
//libuv loop线程函数
void NetcomApi::loopFunction()
{
    if (!bStop)
    {
        uv_idle_start(&idleHandle, &NetcomApi::idle_cb);
        loopStartTime = std::chrono::system_clock::now();
        lastRead = lastWrite = 0;
        uv_run(&loop, UV_RUN_DEFAULT);
    }
    //clear resource
    sendTaskQue.clear();
    sendBuffer.clear();
    recvBuffer.clear();
    //close handles
    uv_close((uv_handle_t *)&tcpHandle, nullptr);
    uv_close((uv_handle_t *)&idleHandle, nullptr);
    uv_close((uv_handle_t *)&timerHandle, nullptr);
}
void NetcomApi::closeConnection()
{
    bConnected = false;
    //清理资源
    sendTaskQue.clear();
    sendBuffer.clear();
    recvBuffer.clear();
    uv_close((uv_handle_t *)&tcpHandle, nullptr);
}
void NetcomApi::connectServer()
{
    if (servAddr[0] == '\0')
    {
        return;
    }
    struct sockaddr_in dest;
    uv_ip4_addr(servAddr, servPort, &dest);
    int r = uv_tcp_connect(&connHandle, &tcpHandle, (const struct sockaddr *)&dest, &NetcomApi::connect_cb);
    if (r < 0)
    {
        fprintf(stderr, "Connect error: %s.\n", uv_strerror(r));
    }
}
void NetcomApi::sendHeartbeat()
{
    
}