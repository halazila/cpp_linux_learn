#include <iostream>
#include <functional>
#include <sys/socket.h>
#include "SolConnServer.h"
#include "SerializableFactory.h"

SolConnServer::SolConnServer(/* args */)
{
    //忽略SIGPIPE信号
    signal(SIGPIPE, SIG_IGN);
    uv_loop_init(&loop);
    uv_tcp_init(&loop, &tcpHandle);
    uv_idle_init(&loop, &idleHandle);
    uv_timer_init(&loop, &hbtTimerHandle);
    tcpHandle.data = this;
    idleHandle.data = this;
    hbtTimerHandle.data = this;
}

SolConnServer::~SolConnServer()
{
    Join();
}

void SolConnServer::Start()
{
    bStop = false;
    uv_tcp_init(&loop, &tcpHandle);
    tcpHandle.data = this;
    loopThread = std::thread(std::bind(&SolConnServer::loopFunction, this));
}
void SolConnServer::Stop()
{
    bStop = true;
}
void SolConnServer::Join()
{
    if (loopThread.joinable())
        loopThread.join();
}
//客户端写注册
void SolConnServer::WeventCtl(SolConnClient *client, int operate)
{
    std::lock_guard<decltype(mtxWevent)> lock(mtxWevent);
    switch (operate)
    {
    case WPOLL_ADD:
        listWevent.push_back(client);
        break;
    case WPOLL_DEL:
        for (auto it = listWevent.begin(); it != listWevent.end(); it++)
        {
            if (*it == client)
            {
                listWevent.erase(it);
            }
        }
    default:
        break;
    }
}

///***private function***///
void SolConnServer::loopFunction()
{
    while (!bStop)
    {
        uv_tcp_init(&loop, &tcpHandle);
        uv_idle_start(&idleHandle, &SolConnServer::idle_cb);
        uv_timer_start(&hbtTimerHandle, &SolConnServer::hbt_timer_cb, 100, nHbtChkInterval);
        loopStartTime = std::chrono::system_clock::now();
        struct sockaddr_in addr;
        uv_ip4_addr("0.0.0.0", nPort, &addr);
        uv_tcp_bind(&tcpHandle, (const struct sockaddr *)&addr, 0);
        uv_listen((uv_stream_t *)&tcpHandle, 128, &SolConnServer::connect_cb);
        uv_run(&loop, UV_RUN_DEFAULT);
    }
}
void SolConnServer::writeHandle()
{
    {
        std::lock_guard<decltype(mtxWevent)> lock(mtxWevent);
        listWevent.swap(listWeventBck);
    }
    SolConnClient *client;
    for (auto it = listWeventBck.begin(); it != listWeventBck.end(); it++)
    {
        client = *it;
        if (client->bConnected)
        {
            CByteArray &byteArr = client->reqBuffer.first;
            client->req_write_buf = uv_buf_init(byteArr.CharArray(), byteArr.RightPos());
            uv_write(&client->write_req, (uv_stream_t *)&client->tcpHandle, &client->req_write_buf, 1, &SolConnServer::req_write_cb);
        }
    }
    listWeventBck.clear();
}
//获取loop开始至今的秒数
int64_t SolConnServer::getNowSeconds()
{
    auto tnow = std::chrono::system_clock::now();
    auto dura = std::chrono::duration_cast<std::chrono::seconds>(tnow - loopStartTime);
    return dura.count();
}
//关闭客户端
//客户端资源销毁在定时器检测回调中完成（删除超时客户端）
void SolConnServer::closeClient(SolConnClient *client)
{
    if (client->bConnected)
        uv_close((uv_handle_t *)&client->tcpHandle, nullptr); //uv_handle_t cannot be closed twice
    client->bConnected = false;
}
//////////////////静态回调函数//////////////////////
void SolConnServer::connect_cb(uv_stream_t *server, int status)
{
    SolConnServer *pServer = (SolConnServer *)server->data;
    if (status < 0)
    {
        fprintf(stderr, "Error on Listening: %s.\n", uv_strerror(status));
    }
    //创建新的客户端
    SolConnClient *ptrClient = new SolConnClient();
    uv_tcp_t *client = &ptrClient->tcpHandle;
    uv_tcp_init(&pServer->loop, client);
    if (uv_accept((uv_stream_t *)server, (uv_stream_t *)&ptrClient->tcpHandle) == 0)
    {
        struct sockaddr_in sin;
        int slen = sizeof(sin);
        //local addr
        uv_tcp_getsockname(client, (sockaddr *)&sin, &slen);
        char sname[32];
        uv_ip4_name(&sin, sname, sizeof sname);
        std::cout << "sockname: " << sname << "sockport: " << ntohs(sin.sin_port) << std::endl;
        //remote addr
        uv_tcp_getpeername(client, (sockaddr *)&sin, &slen);
        uv_ip4_name(&sin, sname, sizeof sname);
        std::cout << "peername: " << sname << "peerport: " << ntohs(sin.sin_port) << std::endl;
        //open read monitor
        int r = uv_read_start((uv_stream_t *)client, &SolConnServer::alloc_cb, &SolConnServer::read_cb);
        if (r < 0)
        {
            fprintf(stderr, "Error on reading client stream: %s.\n", uv_strerror(r));
        }

        char strKey[64] = {0};
        sprintf(strKey, "%s:%d", sname, ntohs(sin.sin_port));
        //store client
        pServer->mapConnClient.insert({strKey, ptrClient});
        //client相关参数初始化
        ptrClient->objKey = strKey;
        ptrClient->data = pServer;
        ptrClient->lastWrite = ptrClient->lastRead = pServer->getNowSeconds();
        ptrClient->bConnected = true;
    }
    else
    {
        uv_close((uv_handle_t *)client, nullptr);
    }
}
void SolConnServer::alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    SolConnClient *client = (SolConnClient *)handle->data;
    buf->len = client->nNeedRead;
    buf->base = client->recvBuffer.CharArray() + client->recvBuffer.RightPos();
}
void SolConnServer::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    SolConnClient *client = (SolConnClient *)stream->data;
    SolConnServer *server = (SolConnServer *)client->data;
    /**/
    if (nread > 0)
    {
        //缓存右指针右移
        client->recvBuffer.RightForward(nread);
        if (nread == client->nNeedRead) //接收到完整数据
        {
            if (client->recvStat == ERecvStatType::Type_RecvHead) //接收到完整包头
            {
                PackageHeadField pkghead = server->pkgCodec.decodeHead(client->recvBuffer);
                //设置下次读取最大长度
                client->nNeedRead = pkghead.pkgBodyLen;
                client->recvStat = ERecvStatType::Type_RecvBody;
                //左指针归零
                client->recvBuffer.LeftBackward(PackageHeadLen);
                if (client->recvBuffer.Capacity() < client->nNeedRead + PackageHeadLen)
                    client->recvBuffer.Reallocate(client->nNeedRead + PackageHeadLen);
            }
            else //接收到完整包体
            {
                //发送应答Ack
                PackageHeadField pkghead = server->pkgCodec.decodeHead(client->recvBuffer);
                //命令处理
                char *pchr = client->recvBuffer.CharArray() + client->recvBuffer.LeftPos();
                uint8_t objType = pchr[0];
                ISerializable *cmd = SerializableFactory::CreateObject((ERequestType)objType);
                server->pkgCodec.decodeSerializeObj(*cmd, client->recvBuffer);
                CommandTask *cmdTask = new CommandTask();
                auto it = server->mapConnClient.find(client->getObjectKey());
                cmdTask->reqRtnType = pkghead.reqRtnType;
                cmdTask->requestID = pkghead.requestID;
                cmdTask->client = client;
                cmdTask->server = server;
                cmdTask->cmd.reset(cmd);
                //暂时使用libuv全局线程池处理
                uv_work_t *newwork = (uv_work_t *)malloc(sizeof(uv_work_t));
                newwork->data = cmdTask;
                uv_queue_work(&server->loop, newwork, &SolConnServer::work_cb, &SolConnServer::after_work_cb);
                //接收缓存状态初始化
                client->recvBuffer.Init();
                //接收状态改变
                client->recvStat = ERecvStatType::Type_RecvHead;
                client->nNeedRead = PackageHeadLen;
            }
        }
        else //接收到部分数据
        {
            client->nNeedRead -= nread;
        }
    }
    else if (nread == 0)
    {
        //Not Error, EAGAIN or EWOULDBLOCK
    }
    else
    {
        //Error
        fprintf(stderr, "Read error: %s.\n", uv_strerror(nread));
        if (nread != UV_ECANCELED)
            server->closeClient(client);
        return;
    }
    client->lastRead = server->getNowSeconds();
}
void SolConnServer::req_write_cb(uv_write_t *req, int status)
{
    SolConnClient *client = (SolConnClient *)req->data;
    SolConnServer *server = (SolConnServer *)client->data;
    client->reqBuffer.first.Init();
    client->reqBuffer.second = false;
    if (status < 0)
    {
        fprintf(stderr, "Write error: %s.\n", uv_strerror(status));
        if (status != UV_ECANCELED)
        {
            SolConnServer *server = (SolConnServer *)client->data;
            server->closeClient(client);
        }
    }
    else
    {
        client->lastWrite = server->getNowSeconds();
    }
}
void SolConnServer::shutdown_cb(uv_shutdown_t *req, int status)
{
}
//keep-alive 检测
void SolConnServer::hbt_timer_cb(uv_timer_t *timer)
{
    SolConnServer *pServer = (SolConnServer *)timer->data;
    uint64_t nowTime = pServer->getNowSeconds();
    uint64_t wIdle, rIdle;
    for (auto it = pServer->mapConnClient.begin(); it != pServer->mapConnClient.end();)
    {
        rIdle = nowTime - it->second->lastRead;
        wIdle = nowTime - it->second->lastWrite;
        if (rIdle > MAX_IDLE_INTERVAL && wIdle > MAX_IDLE_INTERVAL)
        {
            /* code */
            pServer->closeClient(it->second);
            std::cout << "deconstruct client " << it->second->getObjectKey() << std::endl;
            delete it->second;
            it = pServer->mapConnClient.erase(it);
        }
        else
        {
            it++;
        }
    }
}
void SolConnServer::idle_cb(uv_idle_t *handle)
{
    SolConnServer *pServer = (SolConnServer *)handle->data;
    //write req
    pServer->writeHandle();
    //退出
    if (pServer->bStop)
    {
        //write req clear
        pServer->listWevent.clear();
        pServer->listWeventBck.clear();
        //close client socket
        //只关闭，不清理，服务器定时任务中会清理
        for (auto it = pServer->mapConnClient.begin(); it != pServer->mapConnClient.end(); it++)
        {
            if (it->second->bConnected)
                uv_close((uv_handle_t *)&it->second->tcpHandle, nullptr);
            it->second->bConnected = false;
        }
        //关闭监听
        uv_close((uv_handle_t *)&pServer->tcpHandle, nullptr);
        //close timer & idle
        uv_timer_stop(&pServer->hbtTimerHandle);
        std::cout << "idle stop....." << std::endl;
        uv_idle_stop(&pServer->idleHandle);
        std::cout << "stop loop" << std::endl;
        uv_stop(&pServer->loop);
    }
}
void SolConnServer::work_cb(uv_work_t *req)
{
    CommandTask *cmdtask = (CommandTask *)req->data;
    SolConnClient *ptrClient = cmdtask->client;
    if (ptrClient != nullptr)
    {
        ptrClient->CmdHandle(cmdtask->cmd.get());
    }
}
void SolConnServer::after_work_cb(uv_work_t *req, int status)
{
    CommandTask *cmdtask = (CommandTask *)req->data;
    delete cmdtask;
    free(req);
}