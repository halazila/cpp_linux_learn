#include <iostream>
#include "DispManagerImpl.h"

DispManagerImpl::DispManagerImpl(/* args */)
{
}

DispManagerImpl::~DispManagerImpl()
{
}

void DispManagerImpl::OnConnect()
{
    cout << "DispManagerImpl::OnConnect()" << endl;
}

void DispManagerImpl::OnDisconnect()
{
    cout << "DispManagerImpl::OnDisconnect()" << endl;
}

void DispManagerImpl::OnRspQryManageUser(CommResponse *pRsp, ManageUser *pManagerUser, int nRequestId, bool bLast)
{
    if (pRsp && pRsp->ErrorID == 0)
    {
        if (pManagerUser)
        {
            char str[256];
            sprintf(str, "Manager: ManagerName=%s,ManagerPasswd=%s\n",
                    pManagerUser->ManagerName,
                    pManagerUser->ManagerPasswd);
            cout << str;
        }
    }
    else if (pRsp)
    {
        cout << pRsp->ErrMsg << endl;
    }
    if (bLast)
        cout << "requestid " << nRequestId << " all returned" << endl;
}

void DispManagerImpl::OnRspQryServerConfig(CommResponse *pRsp, ServerConfig *pServerCfg, int nRequestId, bool bLast)
{
    if (pRsp && pRsp->ErrorID == 0)
    {
        if (pServerCfg)
        {
            char str[256];
            sprintf(str, "ServerConfig: ServerName=%s,ServerIP=%s,ServerPort=%d\n",
                    pServerCfg->ServerName,
                    pServerCfg->ServerIP,
                    pServerCfg->ServerPort);
            cout << str;
        }
    }
    else if (pRsp)
    {
        cout << pRsp->ErrMsg << endl;
    }
    if (bLast)
        cout << "requestid " << nRequestId << " all returned" << endl;
}

void DispManagerImpl::OnRspQryStrategyConfig(CommResponse *pRsp, StrategyConfig *pStrategyCfg, int nRequestId, bool bLast)
{
    if (pRsp && pRsp->ErrorID == 0)
    {
        if (pStrategyCfg)
        {
            char str[512];
            sprintf(str, "StrategyConfig: ConfigName=%s,PlatformName=%s,StragetyName=%s,SrcRootDir=%s,ManagerName=%s\n",
                    pStrategyCfg->ConfigName,
                    pStrategyCfg->PlatformName,
                    pStrategyCfg->StragetyName,
                    pStrategyCfg->SrcRootDir,
                    pStrategyCfg->ManagerName);
            cout << str;
        }
    }
    else if (pRsp)
    {
        cout << pRsp->ErrMsg << endl;
    }
    if (bLast)
        cout << "requestid " << nRequestId << " all returned" << endl;
}

void DispManagerImpl::OnRspQryDeployConfig(CommResponse *pRsp, DeployConfig *pDeployCfg, int nRequestId, bool bLast)
{
    if (pRsp && pRsp->ErrorID == 0)
    {
        if (pDeployCfg)
        {
            char str[512];
            sprintf(str, "DeployConfig: ConfigName=%s,ServerName=%s,UserName=%s,UserPasswd=%s,DstRootDir=%s,ExeUser=%s,ManagerName=%s\n",
                    pDeployCfg->ConfigName,
                    pDeployCfg->ServerName,
                    pDeployCfg->UserName,
                    pDeployCfg->UserPasswd,
                    pDeployCfg->DstRootDir,
                    pDeployCfg->ExeUser,
                    pDeployCfg->ManagerName);
            cout << str;
        }
    }
    else if (pRsp)
    {
        cout << pRsp->ErrMsg << endl;
    }
    if (bLast)
        cout << "requestid " << nRequestId << " all returned" << endl;
}

void DispManagerImpl::OnRspQryDeployGroup(CommResponse *pRsp, DeployGroup *pDeployGrp, int nRequestId, bool bLast)
{
    if (pRsp && pRsp->ErrorID == 0)
    {
        if (pDeployGrp)
        {
            char str[512];
            sprintf(str, "DeployGroup: GroupName=%s,StragetyConfig=%s,DeployConfig=%s,ManagerName=%s\n",
                    pDeployGrp->GroupName,
                    pDeployGrp->StragetyConfig,
                    pDeployGrp->DeployConfig,
                    pDeployGrp->ManagerName);
            cout << str;
        }
    }
    else if (pRsp)
    {
        cout << pRsp->ErrMsg << endl;
    }
    if (bLast)
        cout << "requestid " << nRequestId << " all returned" << endl;
}

void DispManagerImpl::OnRspCommon(CommResponse *pRsp, int nRequestId)
{
}

void DispManagerImpl::OnRspLogin(CommResponse *pRsp, int nRequestId)
{
    if (pRsp)
    {
        cout << "DispManagerImpl::OnRspLogin" << endl;
        cout << pRsp->ErrMsg << endl;
    }
}

void DispManagerImpl::OnRspLogout(CommResponse *pRsp, int nRequestId)
{
    if (pRsp)
    {
        cout << "DispManagerImpl::OnRspLogout" << endl;
        cout << pRsp->ErrMsg << endl;
    }
}

void DispManagerImpl::OnRspDeploy(CommResponse *pRsp, int nRequestId)
{
}

void DispManagerImpl::OnRspFinish(CommResponse *pRsp, int nRequestId)
{
}

void DispManagerImpl::OnRspInsert(CommResponse *pRsp, int nRequestId)
{
    if (pRsp)
    {
        cout << "Insert request: " << nRequestId << ", Result: " << pRsp->ErrMsg << endl;
    }
}

void DispManagerImpl::OnRspUpdate(CommResponse *pRsp, int nRequestId)
{
    if (pRsp)
    {
        cout << "Update request: " << nRequestId << ", Result: " << pRsp->ErrMsg << endl;
    }
}

void DispManagerImpl::OnRspDelete(CommResponse *pRsp, int nRequestId)
{
    if (pRsp)
    {
        cout << "Delete request: " << nRequestId << ", Result: " << pRsp->ErrMsg << endl;
    }
}
