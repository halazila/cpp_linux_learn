#pragma once

#include "AsyncZmqApi.h"

#define ERROR_SUCCESS 0
#define ERROR_NOCONNECT -1
#define ERROR_PARAMETER -2

class DispatcherManager
{
private:
    AsyncZmqApi azmqApi;
    int requestID = 0;
    std::string m_strServAddr;
    std::string m_strIdentity;

    /////接收状态机///////
    int stateMachine = EMsgRecvState::StatInit;
    STCMsgPattern msgPattern;
    ReqResponse reqRsp;
    int recvCmd = ECommandType::TNullCmd;
    int eleType = EElementType::TNullType;
    /////////////////////
public:
    DispatcherManager();
    ~DispatcherManager();
    void RegistServerAddr(const std::string &strAddr);
    int OpenAsyncApi();
    void CloseAsyncApi();
    int ReqLogin(const std::string &strid);
    int ReqLogout(const std::string &strid);
    ////回调函数
    void OnConnect();
    void OnDisconnect();

private:
    void recvHandle(char *data, int len, bool bLast);
    void onReqRsp(ReqResponse &reqRsp);
    void onReqRsp(ReqResponse &reqRsp, int nEleType, char *data, int len, bool bLast);
    void onMsgPush(int cmd);
    void onMsgPush(int cmd, int nEleType, char *data, int len, bool bLast);
};
