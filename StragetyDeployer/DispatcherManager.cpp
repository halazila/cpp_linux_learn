#include "DispatcherManager.h"

DispatcherManager::DispatcherManager(/* args */)
{
    azmqApi.SetRecvCallback(std::bind(&DispatcherManager::recvHandle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    azmqApi.SetConnectCallback(std::bind(&DispatcherManager::OnConnect, this));
    azmqApi.SetConnectCallback(std::bind(&DispatcherManager::OnDisconnect, this));
}

DispatcherManager::~DispatcherManager()
{
}

void DispatcherManager::RegistServerAddr(const std::string &strAddr)
{
    m_strServAddr = strAddr;
    azmqApi.SetRemoteAddr(strAddr);
}

int DispatcherManager::OpenAsyncApi()
{
    int ret = azmqApi.Connect();
    if (ret == ASYNCZMQAPI_OK)
    {
        azmqApi.Start();
    }
    return ret;
}

void DispatcherManager::CloseAsyncApi()
{
    azmqApi.Stop();
}

int DispatcherManager::ReqLogin(const std::string &strid)
{
    if (strid.empty())
    {
        return ERROR_PARAMETER;
    }
    if (!azmqApi.IsConnected())
    {
        return ERROR_NOCONNECT;
    }
    m_strIdentity = strid;
    zmq::socket_t sock = azmqApi.InProcSocket();
    // managerid->cmd->requestid
    azmqApi.Send(sock, strid.c_str(), strid.length(), false);
    int cmd = ECommandType::TLogin;
    azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
    azmqApi.Send(sock, (char *)&requestID, sizeof(int));
    requestID++;
    return ERROR_SUCCESS;
}

int DispatcherManager::ReqLogout(const std::string &strid)
{
    if (!azmqApi.IsConnected())
    {
        return ERROR_NOCONNECT;
    }
    m_strIdentity = strid;
    zmq::socket_t sock = azmqApi.InProcSocket();
    // managerid->cmd->requestid
    azmqApi.Send(sock, strid.c_str(), strid.length(), false);
    int cmd = ECommandType::TExit;
    azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
    azmqApi.Send(sock, (char *)&requestID, sizeof(int));
    requestID++;
    return ERROR_SUCCESS;
}

void DispatcherManager::OnConnect()
{
}

void DispatcherManager::OnDisconnect()
{
}

void DispatcherManager::recvHandle(char *data, int len, bool bLast)
{
    switch (stateMachine)
    {
    case EMsgRecvState::StatInit:
    {
        assert(len == sizeof(msgPattern));
        memcpy(&msgPattern, data, sizeof(msgPattern));
        if (!bLast)
            stateMachine = EMsgRecvState::StatGetPattern;
    }
    break;
    case EMsgRecvState::StatGetPattern:
    {
        if (msgPattern == STCMsgPattern::TPassiveResponse) //应答消息
        {
            assert(len == sizeof(reqRsp));
            memcpy(&reqRsp, data, sizeof(reqRsp));
            if (bLast)
            {
                stateMachine = EMsgRecvState::StatInit;
                onReqRsp(reqRsp);
            }
            else
            {
                stateMachine = EMsgRecvState::StatGetRsp;
            }
        }
        else //推送消息
        {
            assert(len == sizeof(recvCmd));
            memcpy(&recvCmd, data, sizeof(recvCmd));
            if (bLast)
            {
                stateMachine = EMsgRecvState::StatInit;
                onMsgPush(recvCmd);
            }
            else
            {
                stateMachine = EMsgRecvState::StatGetCmd;
            }
        }
    }
    break;
    case EMsgRecvState::StatGetCmd:
    {
        assert(len == sizeof(eleType));
        memcpy(&eleType, data, sizeof(eleType));
        if (bLast)
        {
            stateMachine = EMsgRecvState::StatInit;
            onMsgPush(recvCmd, eleType, nullptr, 0, bLast);
        }
        else
        {
            stateMachine = EMsgRecvState::StatGetEleType;
        }
    }
    break;
    case EMsgRecvState::StatGetRsp:
    {
        assert(len == sizeof(eleType));
        memcpy(&eleType, data, sizeof(eleType));
        if (bLast)
        {
            stateMachine = EMsgRecvState::StatInit;
            onReqRsp(reqRsp, eleType, nullptr, 0, bLast);
        }
        else
        {
            stateMachine = EMsgRecvState::StatGetEleType;
        }
    }
    break;
    case EMsgRecvState::StatGetEleType:
    {
        if (bLast)
            stateMachine = EMsgRecvState::StatInit;
        if (msgPattern == STCMsgPattern::TPassiveResponse) //应答消息
        {
            onReqRsp(reqRsp, eleType, data, len, bLast);
        }
        else //推送消息
        {
            onMsgPush(recvCmd, eleType, data, len, bLast);
        }
    }
    break;
    default:
        break;
    }
}

void DispatcherManager::onReqRsp(ReqResponse &reqRsp)
{
}

void DispatcherManager::onReqRsp(ReqResponse &reqRsp, int nEleType, char *data, int len, bool bLast)
{
}

void DispatcherManager::onMsgPush(int cmd)
{
    switch (cmd)
    {
    case ECommandType::TKeepAlive:
    {
        zmq::socket_t sock = azmqApi.InProcSocket();
        int cmd = ECommandType::TKeepAlive;
        azmqApi.Send(sock, m_strIdentity.c_str(), m_strIdentity.length(), false);
        azmqApi.Send(sock, (const char *)&cmd, sizeof(cmd));
    }
    break;
    default:
        break;
    }
}

void DispatcherManager::onMsgPush(int cmd, int nEleType, char *data, int len, bool bLast)
{
}
