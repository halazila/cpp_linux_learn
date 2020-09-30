#include <functional>
// #include <stdlib.h>
#include <string.h>
#include <iostream>
#include "DispatchServer.h"

////////////////////////////////////////////////////////////////
//init所有handle
void ClientConn::initHandle(uv_loop_t *loop)
{
    uv_tcp_init(loop, &tcpHandle);
    uv_idle_init(loop, &idleHandle);
    uv_timer_init(loop, &timerHandle);
}
//close所有handle
void ClientConn::closeHandle()
{
    uv_close((uv_handle_t *)&tcpHandle, nullptr);
    uv_close((uv_handle_t *)&idleHandle, nullptr);
    uv_close((uv_handle_t *)&timerHandle, nullptr);
}
//缓存收到的字节
void ClientConn::appendRecvBytes(const char *pChr, const int len)
{
    BytesLinkedBuf *newBuf = new BytesLinkedBuf;
    newBuf->byteBuffer = (char *)pChr;
    newBuf->usedSize = len;
    if (readBuf)
    {
        BytesLinkedBuf *p = readBuf;
        while (p->ptrNext)
        {
            p = p->ptrNext;
        }
        p->ptrNext = newBuf;
    }
    else
    {
        readBuf = newBuf;
    }
    readBytes += len;
    bReadUpd = true;
}
//return -1,失败，其他；解析的通信包数目
int ClientConn::parsePackage()
{
    int nRet = 0;
   

    return nRet;
}
////////////////////////////////////////////////////////////////
DispatchServer *DispatchServer::pGlobalInstance = nullptr;
std::mutex DispatchServer::globalMtx;

DispatchServer::DispatchServer(/* args */)
{
    loop = new uv_loop_t;
}

DispatchServer::~DispatchServer()
{
    delete loop;
    loop = nullptr;
}

DispatchServer *DispatchServer::getInstance()
{
    if (nullptr == pGlobalInstance)
    {
        std::lock_guard<decltype(DispatchServer::globalMtx)> lock(DispatchServer::globalMtx);
        if (nullptr == pGlobalInstance)
        {
            pGlobalInstance = new DispatchServer();
        }
    }
    return pGlobalInstance;
}
void DispatchServer::delInstance()
{
    if (nullptr != pGlobalInstance)
    {
        std::lock_guard<decltype(DispatchServer::globalMtx)> lock(DispatchServer::globalMtx);
        if (nullptr != pGlobalInstance)
        {
            delete pGlobalInstance;
            pGlobalInstance = nullptr;
        }
    }
}

void DispatchServer::connection_cb(uv_stream_t *server, int status)
{
    DispatchServer *pServer = getInstance();
    if (status < 0)
    {
        fprintf(stderr, "Line: %d, Error on Listening: %s.\n", __LINE__, uv_strerror(status));
    }

    ClientConn *conn = new ClientConn;
    conn->initHandle(pServer->loop);

    if (uv_accept(server, (uv_stream_t *)&conn->tcpHandle) < 0)
    {
        conn->closeHandle();
        delete conn;
        return;
    }
    if (pServer->addClientConn(conn) != DISPATCHER_OK)
    {
        conn->closeHandle();
        delete conn;
        return;
    }
    int r = uv_read_start((uv_stream_t *)&conn->tcpHandle, &DispatchServer::alloc_cb, &DispatchServer::read_cb);
    if (r < 0)
    {
        fprintf(stderr, "Line: %d, Error on reading client stream: %s.\n", __LINE__, uv_strerror(r));
    }
}

void DispatchServer::alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
    // ClientConn *conn = (ClientConn *)handle->data;
    buf->len = size;
    buf->base = new char[buf->len];
    memset(buf->base, 0, size);
}

void DispatchServer::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0)
    {
        delete buf->base;
        uv_shutdown_t *reqShutdown = new uv_shutdown_t;
        uv_shutdown(reqShutdown, stream, &DispatchServer::shutdown_cb);
    }
    else if (nread == 0)
    {
        delete buf->base;
    }
    else
    {
        DispatchServer *pServer = getInstance();
        pServer->readProcess(stream, nread, buf);
    }
}
void DispatchServer::write_cb(uv_write_t *req, int status)
{
    for (int i = 0; i < req->nbufs; i++)
    {
        delete[] req->bufs[i].base;
    }
    delete req;
}

void DispatchServer::timer_cb(uv_timer_t *timer)
{
}

void DispatchServer::shutdown_cb(uv_shutdown_t *req, int status)
{
    uv_stream_t *stream = req->handle;
    DispatchServer *pServer = getInstance();
    ClientConn *conn = (ClientConn *)stream->data;
    pServer->removeClientConn(conn);
    conn->closeHandle();
    delete conn;
    delete req;
}

//解析所有缓存的数据
void DispatchServer::idle_cb(uv_idle_t *handle)
{
    ClientConn *conn = (ClientConn *)handle->data;
    DispatchServer *pServer = getInstance();
    if (conn->parsePackage() == PARSE_PACKAGE_ERR)
    {
        conn->closeHandle();
        return;
    }
    pServer->commandProcess(conn);
    uv_idle_stop(handle);
}

int DispatchServer::getClientKey(uv_tcp_t *client, std::string &strKey)
{
    int ret = DISPATCHER_OK;
    struct sockaddr_in sin;
    int slen = sizeof(sin);
    uv_tcp_getpeername(client, (struct sockaddr *)&sin, &slen);
    if (sin.sin_family != AF_INET)
    {
        return DISPATCHER_ERR;
    }
    char ipaddr[16] = {0};
    int port;
    uv_ip4_name(&sin, ipaddr, sizeof ipaddr);
    port = ntohs(sin.sin_port);
    //instore connection
    char str[32] = {0};
    sprintf(str, "%s:%d", ipaddr, port);
    strKey = str;
    return ret;
}

int DispatchServer::addClientConn(ClientConn *conn)
{
    int ret = DISPATCHER_OK;
    if (getClientKey(&conn->tcpHandle, conn->clientKey) != DISPATCHER_OK)
    {
        return DISPATCHER_ERR;
    }
    {
        std::lock_guard<decltype(m_mtxMapClientConns)> lock(m_mtxMapClientConns);
        m_mapClientConns[conn->clientKey] = conn;
    }
    return ret;
}

int DispatchServer::removeClientConn(ClientConn *conn)
{
    int ret = DISPATCHER_OK;
    std::string strKey = conn->clientKey;
    if (strKey.empty())
    {
        getClientKey(&conn->tcpHandle, strKey);
    }
    std::lock_guard<decltype(m_mtxMapClientConns)> lock(m_mtxMapClientConns);
    auto it = m_mapClientConns.find(strKey);
    if (it != m_mapClientConns.end())
    {
        m_mapClientConns.erase(it);
    }
    return ret;
}

int DispatchServer::readProcess(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    ClientConn *conn = (ClientConn *)stream->data;
    conn->appendRecvBytes(buf->base, buf->len);
    uv_idle_start(&conn->idleHandle, DispatchServer::idle_cb);
    delete buf; //buf.base switched to conn
}
void DispatchServer::commandProcess(ClientConn *conn)
{
}
