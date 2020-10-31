#pragma once

#include "uv.h"
#include "SillyCommand.h"
#include "CachedBuffer.h"
#include "CommonStruct.h"

//服务器客户端连接对象类
class SolConnClient
{
public:
    //预留数据域，SolClientConn内部不会用到
    void *data;

private:
    //发送消息序列号，递增
    uint64_t serialID = 0;
    //Req发送缓存<发送缓存,是否有Req待发送>
    std::pair<CByteArray, bool> reqBuffer;
    //Ack发送管理<发送缓存,是否有Ack待发送>
    std::pair<CByteArray, bool> ackBuffer;
    //接收字节缓存
    CByteArray recvBuffer;
    //接收状态
    ERecvStatType recvStat = ERecvStatType::Type_RecvHead;
    //下次需要接收字节数
    int nNeedRead = PackageHeadLen;
    //上次读成功时间
    int64_t lastRead;
    //上次写成功时间
    int64_t lastWrite;
    //tcp handle
    uv_tcp_t tcpHandle;
    uv_write_t write_req;
    uv_buf_t req_write_buf;

    bool bConnected = false;
    //IP:Port
    std::string objKey;

public:
    SolConnClient(/* args */);
    ~SolConnClient();
    //命令处理
    void CmdHandle(ISerializable *cmd);
    //获取客户端key
    std::string getObjectKey();
    //try post message, return when success or timeout milliseconds
    //return value: 0-success, -1-timeout, -2-other error

private:
    /**/
    int postMsg(ISerializable &msg, int timeout, int requestID = 0, EPkgReqRtnType reqRtn = EPkgReqRtnType::Type_Req);

    friend class SolConnServer;
};
