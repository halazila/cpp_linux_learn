#include "DispatcherManager.h"

DispatcherManager::DispatcherManager(/* args */)
{
    azmqApi.SetRecvCallback(std::bind(&DispatcherManager::recvHandle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    azmqApi.SetConnectCallback(std::bind(&DispatcherManager::OnConnect, this));
    azmqApi.SetDisconnectCallback(std::bind(&DispatcherManager::OnDisconnect, this));
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

int DispatcherManager::ReqLogin(const std::string &strid, int nRequestId)
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
    azmqApi.Send(sock, (char *)&nRequestId, sizeof(int));
    return ERROR_SUCCESS;
}

int DispatcherManager::ReqLogout(int nRequestId)
{
    if (!azmqApi.IsConnected())
    {
        return ERROR_NOCONNECT;
    }
    zmq::socket_t sock = azmqApi.InProcSocket();
    // managerid->cmd->requestid
    azmqApi.Send(sock, m_strIdentity.c_str(), m_strIdentity.length(), false);
    int cmd = ECommandType::TLogout;
    azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
    azmqApi.Send(sock, (char *)&nRequestId, sizeof(int));
    return ERROR_SUCCESS;
}

int DispatcherManager::ReqQryByColumnFilter(const vector<ColumnFilter> &filterVec, int eleType, int nRequestId)
{
    if (!azmqApi.IsConnected())
    {
        return ERROR_NOCONNECT;
    }
    zmq::socket_t sock = azmqApi.InProcSocket();
    // managerid->cmd->requestid->eletype->elements
    azmqApi.Send(sock, m_strIdentity.c_str(), m_strIdentity.length(), false);
    int cmd = ECommandType::TQuery;
    azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
    azmqApi.Send(sock, (char *)&nRequestId, sizeof(int), false);
    azmqApi.Send(sock, (char *)&eleType, sizeof(eleType), filterVec.size() == 0);
    int n = filterVec.size();
    for (int i = 0; i < n; i++)
    {
        azmqApi.Send(sock, filterVec[i].ColName.c_str(), filterVec[i].ColName.size(), false);
        azmqApi.Send(sock, filterVec[i].ColVal.c_str(), filterVec[i].ColVal.size(), i == n - 1);
    }
    return ERROR_SUCCESS;
}

int DispatcherManager::ReqDeployExecuteFinish(const vector<DeployGroup> &depGrpVec, int nRequestId, int actType)
{
    if (!azmqApi.IsConnected())
    {
        return ERROR_NOCONNECT;
    }
    zmq::socket_t sock = azmqApi.InProcSocket();
    // managerid->cmd->requestid->eletype->elements
    azmqApi.Send(sock, m_strIdentity.c_str(), m_strIdentity.length(), false);
    int cmd;
    switch (actType)
    {
    case 0:
        cmd = ECommandType::TDeploy;
        break;
    case 1:
        cmd = ECommandType::TExecute;
        break;
    case 2:
        cmd = ECommandType::TFinish;
        break;

    default:
        break;
    }
    azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
    azmqApi.Send(sock, (char *)&nRequestId, sizeof(int), false);
    int eleType = EElementType::TDeployGroup;
    azmqApi.Send(sock, (char *)&eleType, sizeof(eleType), depGrpVec.size() == 0);
    int n = depGrpVec.size();
    for (int i = 0; i < n; i++)
    {
        azmqApi.Send(sock, (char *)&depGrpVec[i], sizeof(DeployGroup), i == n - 1);
    }
    return ERROR_SUCCESS;
}

void DispatcherManager::OnConnect()
{
    cout << "DispatcherManager::OnConnect()" << endl;
}

void DispatcherManager::OnDisconnect()
{
    cout << "DispatcherManager::OnDisconnect()" << endl;
}

void DispatcherManager::OnRspQryManageUser(CommResponse *pRsp, ManageUser *pManagerUser, int nRequestId, bool bLast)
{
}

void DispatcherManager::OnRspQryServerConfig(CommResponse *pRsp, ServerConfig *pServerCfg, int nRequestId, bool bLast)
{
}

void DispatcherManager::OnRspQryStrategyConfig(CommResponse *pRsp, StrategyConfig *pStrategyCfg, int nRequestId, bool bLast)
{
}

void DispatcherManager::OnRspQryDeployConfig(CommResponse *pRsp, DeployConfig *pDeployCfg, int nRequestId, bool bLast)
{
}

void DispatcherManager::OnRspQryDeployGroup(CommResponse *pRsp, DeployGroup *pDeployGrp, int nRequestId, bool bLast)
{
}

void DispatcherManager::OnRspCommon(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspLogin(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspLogout(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspDeploy(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspFinish(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspInsert(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspUpdate(CommResponse *pRsp, int nRequestId)
{
}

void DispatcherManager::OnRspDelete(CommResponse *pRsp, int nRequestId)
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
        assert(len == sizeof(recvEleType));
        memcpy(&recvEleType, data, sizeof(recvEleType));
        if (bLast)
        {
            stateMachine = EMsgRecvState::StatInit;
            onMsgPush(recvCmd, recvEleType, nullptr, 0, bLast);
        }
        else
        {
            stateMachine = EMsgRecvState::StatGetEleType;
        }
    }
    break;
    case EMsgRecvState::StatGetRsp:
    {
        assert(len == sizeof(recvEleType));
        memcpy(&recvEleType, data, sizeof(recvEleType));
        if (bLast)
        {
            stateMachine = EMsgRecvState::StatInit;
            onReqRsp(reqRsp, recvEleType, nullptr, 0, bLast);
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
            onReqRsp(reqRsp, recvEleType, data, len, bLast);
        }
        else //推送消息
        {
            onMsgPush(recvCmd, recvEleType, data, len, bLast);
        }
    }
    break;
    default:
        break;
    }
}

void DispatcherManager::onReqRsp(ReqResponse &reqRsp)
{
    switch (reqRsp.CmdType)
    {
    case ECommandType::TLogin:
        OnRspLogin(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TLogout:
        OnRspLogout(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TDeploy:
        OnRspDeploy(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TFinish:
        OnRspFinish(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TInsert:
        OnRspInsert(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TDelete:
        OnRspDelete(&reqRsp, reqRsp.RequestID);
        break;
    case ECommandType::TUpdate:
        OnRspUpdate(&reqRsp, reqRsp.RequestID);
        break;
    default:
        OnRspCommon(&reqRsp, reqRsp.RequestID);
        break;
    }
}

void DispatcherManager::onReqRsp(ReqResponse &reqRsp, int nEleType, char *data, int len, bool bLast)
{
    switch (reqRsp.CmdType)
    {
    case ECommandType::TQuery:
        switch (nEleType)
        {
        case EElementType::TManageUser:
            assert(len == sizeof(ManageUser));
            OnRspQryManageUser(&reqRsp, (ManageUser *)data, reqRsp.RequestID, bLast);
            break;
        case EElementType::TServerConfig:
            assert(len == sizeof(ServerConfig));
            OnRspQryServerConfig(&reqRsp, (ServerConfig *)data, reqRsp.RequestID, bLast);
            break;
        case EElementType::TStrategyConfig:
            assert(len == sizeof(StrategyConfig));
            OnRspQryStrategyConfig(&reqRsp, (StrategyConfig *)data, reqRsp.RequestID, bLast);
            break;
        case EElementType::TDeployConfig:
            assert(len == sizeof(DeployConfig));
            OnRspQryDeployConfig(&reqRsp, (DeployConfig *)data, reqRsp.RequestID, bLast);
            break;
        case EElementType::TDeployGroup:
            assert(len == sizeof(DeployGroup));
            OnRspQryDeployGroup(&reqRsp, (DeployGroup *)data, reqRsp.RequestID, bLast);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
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
        // std::cout << "receive keep-alive cmd" << std::endl;
    }
    break;
    default:
        break;
    }
}

void DispatcherManager::onMsgPush(int cmd, int nEleType, char *data, int len, bool bLast)
{
}
