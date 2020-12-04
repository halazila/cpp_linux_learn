#include "DispatcherManager.h"

class DispManagerImpl : public DispatcherManager
{
private:
    /* data */
public:
    int m_nRequestID = 0;

public:
    DispManagerImpl(/* args */);
    ~DispManagerImpl();
    void OnConnect() override;
    void OnDisconnect() override;
    void OnRspQryManageUser(CommResponse *pRsp, ManageUser *pManagerUser, int nRequestId, bool bLast) override;
    void OnRspQryServerConfig(CommResponse *pRsp, ServerConfig *pServerCfg, int nRequestId, bool bLast) override;
    void OnRspQryStrategyConfig(CommResponse *pRsp, StrategyConfig *pStrategyCfg, int nRequestId, bool bLast) override;
    void OnRspQryDeployConfig(CommResponse *pRsp, DeployConfig *pDeployCfg, int nRequestId, bool bLast) override;
    void OnRspQryDeployGroup(CommResponse *pRsp, DeployGroup *pDeployGrp, int nRequestId, bool bLast) override;
    void OnRspCommon(CommResponse *pRsp, int nRequestId) override;
    void OnRspLogin(CommResponse *pRsp, int nRequestId) override;
    void OnRspLogout(CommResponse *pRsp, int nRequestId) override;
    void OnRspDeploy(CommResponse *pRsp, int nRequestId) override;
    void OnRspFinish(CommResponse *pRsp, int nRequestId) override;
    void OnRspInsert(CommResponse *pRsp, int nRequestId) override;
    void OnRspUpdate(CommResponse *pRsp, int nRequestId) override;
    void OnRspDelete(CommResponse *pRsp, int nRequestId) override;
};
