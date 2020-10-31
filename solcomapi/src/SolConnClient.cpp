#include <iostream>
#include <thread>
#include <chrono>
#include "SolConnClient.h"
#include "SolConnServer.h"

SolConnClient::SolConnClient(/* args */)
{
    tcpHandle.data = this;
    write_req.data = this;
    nNeedRead = PackageHeadLen;
}
SolConnClient::~SolConnClient()
{
}
//命令处理
void SolConnClient::CmdHandle(ISerializable *cmd)
{
    switch (cmd->nType)
    {
    case ERequestType::TypeReq_Heartbeat:
    {
        std::cout << "Receive heart beat" << std::endl;
        HeartbeatCommand hbtCom;
        postMsg(hbtCom, 1);
    }
    break;
    case ERequestType::TypeReq_TestCmd:
    {
        ((TestCommand *)cmd)->Print();
        TestCommand tstcmd;
        postMsg(tstcmd, 1);
    }
    break;
    /**other command types**/
    default:
        break;
    }
}
//获取客户端key
std::string SolConnClient::getObjectKey()
{
    return objKey;
}

////****private member function***//////
//post message, return value: 0--success, -1--write buffer overflow, -2--network error
int SolConnClient::postMsg(ISerializable &msg, int timeout, int requestID, EPkgReqRtnType reqRtn)
{
    SolConnServer *pServer = (SolConnServer *)data;
    if (bConnected && pServer && !pServer->bStop)
    {
        int ncouts = 0;
        while (reqBuffer.second && ncouts < timeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (reqBuffer.second)
        {
            return -1;
        }
        CByteArray &byteArr = reqBuffer.first;
        reqBuffer.second = true;
        byteArr.Init();
        if (byteArr.Capacity() < msg.dataSize() + PackageHeadLen)
            byteArr.Reallocate(msg.dataSize() + PackageHeadLen);
        PackageHeadField pkghead;
        memset(&pkghead, 0, sizeof(pkghead));
        pkghead.requestID = requestID;
        pkghead.reqRtnType = reqRtn;
        //encode
        byteArr.RightForward(sizeof(pkghead));
        byteArr.LeftForward(sizeof(pkghead));
        pServer->pkgCodec.encodeSerializeObj(msg, byteArr);
        pkghead.pkgBodyLen = byteArr.RightPos() - sizeof(pkghead);
        byteArr.Prepend(pkghead);
        pServer->WeventCtl(this, WPOLL_ADD);
        return 0;
    }
    return -2; //
}