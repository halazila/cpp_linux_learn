#pragma once

#include <vector>
#include "AsyncZmqApi.h"

using namespace std;

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
    int recvEleType = EElementType::TNullType;
    /////////////////////
public:
    DispatcherManager();
    ~DispatcherManager();
    void RegistServerAddr(const std::string &strAddr);
    int OpenAsyncApi();
    void CloseAsyncApi();
    int ReqLogin(const std::string &strid, int nRequestId);
    int ReqLogout(int nRequestId);
    ///query/delete, qrydel: 0-query, 1-delete
    int ReqQryDelByColumnFilter(const vector<ColumnFilter> &filterVec, int eleType, int nRequestId, int qrydel);
    ///insert/update, insupd: 0-insert, 1-update
    template <class T>
    int ReqInsUpdByEletype(const vector<T> &eleVec, int eleType, int nRequestId, int insupd)
    {
        if (eleVec.size() == 0)
            return ERROR_PARAMETER;
        if (!azmqApi.IsConnected())
        {
            return ERROR_NOCONNECT;
        }
        zmq::socket_t sock = azmqApi.InProcSocket();
        // managerid->cmd->requestid->eletype->data
        azmqApi.Send(sock, m_strIdentity.c_str(), m_strIdentity.length(), false);
        int cmd = insupd == 0 ? ECommandType::TInsert : ECommandType::TUpdate;
        azmqApi.Send(sock, (char *)&cmd, sizeof(cmd), false);
        azmqApi.Send(sock, (char *)&requestID, sizeof(int), false);
        azmqApi.Send(sock, (char *)&eleType, sizeof(eleType), false);
        int n = eleVec.size();
        for (int i = 0; i < n; i++)
        {
            azmqApi.Send(sock, (char *)&eleVec[i], sizeof(T), i == n - 1)
        }
        return ERROR_SUCCESS;
    }
    ///Deploy/Execute/Finish, actType: 0-Deploy, 1-Execute, 2-Finish
    int ReqDeployExecuteFinish(const vector<DeployGroup> &depGrpVec, int nRequestId, int actType);

    ////回调函数
    void OnConnect();
    void OnDisconnect();
    ////查询回调
    virtual void OnRspQryManageUser(CommResponse *pRsp, ManageUser *pManagerUser, int nRequestId, bool bLast);
    virtual void OnRspQryServerConfig(CommResponse *pRsp, ServerConfig *pServerCfg, int nRequestId, bool bLast);
    virtual void OnRspQryStrategyConfig(CommResponse *pRsp, StrategyConfig *pStrategyCfg, int nRequestId, bool bLast);
    virtual void OnRspQryDeployConfig(CommResponse *pRsp, DeployConfig *pDeployCfg, int nRequestId, bool bLast);
    virtual void OnRspQryDeployGroup(CommResponse *pRsp, DeployGroup *pDeployGrp, int nRequestId, bool bLast);
    ////login/exit/insert/update/delete/deploy/finish
    virtual void OnRspCommon(CommResponse *pRsp, int nRequestId);
    virtual void OnRspLogin(CommResponse *pRsp, int nRequestId);
    virtual void OnRspLogout(CommResponse *pRsp, int nRequestId);
    virtual void OnRspDeploy(CommResponse *pRsp, int nRequestId);
    virtual void OnRspFinish(CommResponse *pRsp, int nRequestId);
    virtual void OnRspInsert(CommResponse *pRsp, int nRequestId, int nEleType);
    virtual void OnRspUpdate(CommResponse *pRsp, int nRequestId, int nEleType);
    virtual void OnRspDelete(CommResponse *pRsp, int nRequestId, int nEleType);

private:
    void recvHandle(char *data, int len, bool bLast);
    void onReqRsp(ReqResponse &reqRsp);
    void onReqRsp(ReqResponse &reqRsp, int nEleType, char *data, int len, bool bLast);
    void onMsgPush(int cmd);
    void onMsgPush(int cmd, int nEleType, char *data, int len, bool bLast);
};
