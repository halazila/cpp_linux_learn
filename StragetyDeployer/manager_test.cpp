#include <thread>
#include <iostream>
#include "DispatcherManager.h"
#include "AsyncZmqApi.h"
#include "DispManagerImpl.h"

int main(int argc, char *argv[])
{
    DispManagerImpl dpimpl;
    int nRequestID = 0;
    dpimpl.RegistServerAddr("tcp://127.0.0.1:9859");
    dpimpl.OpenAsyncApi();
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    int nlogin = dpimpl.ReqLogin("admin", nRequestID++);
    while (nlogin == ERROR_NOCONNECT)
    {
        nlogin = dpimpl.ReqLogin("admin", nRequestID);
    };

#pragma region query_manager
    vector<ColumnFilter> filterVec;
    ColumnFilter filter;
    filter.ColName = "Wonder";
    filter.ColVal = "admin";
    filterVec.push_back(filter);
    dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TManageUser, nRequestID++);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
#pragma endregion
#pragma region del_by_column
    filterVec.clear();
    filter.ColName = "Wonder";
    filter.ColVal = "testUser19";
    filterVec.push_back(filter);
    dpimpl.ReqDelByColumnFilter(filterVec, EElementType::TStrategyConfig, nRequestID++);
    dpimpl.ReqDelByColumnFilter(filterVec, EElementType::TDeployConfig, nRequestID++);
    dpimpl.ReqDelByColumnFilter(filterVec, EElementType::TDeployGroup, nRequestID++);
#pragma endregion

// #pragma region delete_test
//     vector<ManageUser> vecManage;
//     ManageUser mauser;
//     for (size_t i = 0; i < 10; i++)
//     {
//         memset(&mauser, 0, sizeof(mauser));
//         sprintf(mauser.ManagerName, "testUser%d", i + 10);
//         vecManage.push_back(mauser);
//     }
//     cout << "ManageUser delete request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<ManageUser>(vecManage, EElementType::TManageUser, nRequestID++, ECommandType::TDelete);
//     vector<ServerConfig> vecServer;
//     ServerConfig servCfg;
//     for (size_t i = 0; i < 10; i++)
//     {
//         memset(&servCfg, 0, sizeof(mauser));
//         sprintf(servCfg.ServerIP, "192.168.2.%d", i + 10);
//         sprintf(servCfg.ServerName, "Lan%d", i);
//         servCfg.ServerPort = 95863;
//         vecServer.push_back(servCfg);
//     }
//     cout << "ServerConfig delete request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<ServerConfig>(vecServer, EElementType::TServerConfig, nRequestID++, ECommandType::TDelete);
//     vector<StrategyConfig> vecStragety;
//     StrategyConfig stratyCfg;
//     for (size_t i = 0; i < 10; i++)
//     {
//         memset(&stratyCfg, 0, sizeof(mauser));
//         sprintf(stratyCfg.ConfigName, "config_%d", i);
//         sprintf(stratyCfg.PlatformName, "platform_%d", i);
//         sprintf(stratyCfg.StragetyName, "stragety_%d", i);
//         sprintf(stratyCfg.SrcRootDir, "/home/stragety_%d", i);
//         sprintf(stratyCfg.ManagerName, "testUser%d", i + 10);
//         vecStragety.push_back(stratyCfg);
//     }
//     cout << "StrategyConfig delete request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<StrategyConfig>(vecStragety, EElementType::TStrategyConfig, nRequestID++, ECommandType::TDelete);
//     vector<DeployConfig> vecDeployCfg;
//     DeployConfig deployCfg;
//     for (size_t i = 0; i < 5; i++)
//     {
//         memset(&deployCfg, 0, sizeof(mauser));
//         sprintf(deployCfg.ConfigName, "deploy_cfg_%d", i);
//         sprintf(deployCfg.ServerName, "server_%d", rand() % 10);
//         sprintf(deployCfg.UserName, "trader_%d", i);
//         sprintf(deployCfg.UserPasswd, "trader_pass_%d", i);
//         sprintf(deployCfg.DstRootDir, "/home/trader_%d", i);
//         sprintf(deployCfg.ExeUser, "executer_%d", i);
//         sprintf(deployCfg.ManagerName, "testUser%d", 10 + rand() % 10);
//         vecDeployCfg.push_back(deployCfg);
//     }
//     cout << "DeployConfig delete request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<DeployConfig>(vecDeployCfg, EElementType::TDeployConfig, nRequestID++, ECommandType::TDelete);
//     vector<DeployGroup> vecDeployGrp;
//     DeployGroup deployGrp;
//     for (size_t i = 0; i < 20; i++)
//     {
//         memset(&deployGrp, 0, sizeof(mauser));
//         sprintf(deployGrp.GroupName, "deploy_group_%d", i);
//         sprintf(deployGrp.StragetyConfig, "stragety_%d", rand() % 10);
//         sprintf(deployGrp.DeployConfig, "deploy_cfg_%d", i);
//         sprintf(deployGrp.ManagerName, "testUser%d", 10 + rand() % 10);
//         vecDeployGrp.push_back(deployGrp);
//     }
//     cout << "DeployGroup delete request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<DeployGroup>(vecDeployGrp, EElementType::TDeployGroup, nRequestID++, ECommandType::TDelete);
//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
// #pragma endregion

// #pragma region insert_test
//     cout << "ManageUser insert request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<ManageUser>(vecManage, EElementType::TManageUser, nRequestID++, ECommandType::TInsert);
//     cout << "ServerConfig insert request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<ServerConfig>(vecServer, EElementType::TServerConfig, nRequestID++, ECommandType::TInsert);
//     cout << "StrategyConfig insert request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<StrategyConfig>(vecStragety, EElementType::TStrategyConfig, nRequestID++, ECommandType::TInsert);
//     cout << "DeployConfig insert request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<DeployConfig>(vecDeployCfg, EElementType::TDeployConfig, nRequestID++, ECommandType::TInsert);
//     cout << "DeployGroup insert request, id=" << nRequestID << endl;
//     dpimpl.ReqInsUpdDelByEletype<DeployGroup>(vecDeployGrp, EElementType::TDeployGroup, nRequestID++, ECommandType::TInsert);
//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
// #pragma endregion

// #pragma region query_test
//     filterVec.clear();
//     cout << "ManageUser query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TManageUser, nRequestID++);
//     cout << "ServerConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TServerConfig, nRequestID++);
//     cout << "StrategyConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TStrategyConfig, nRequestID++);
//     cout << "DeployConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TDeployConfig, nRequestID++);
//     cout << "DeployGroup query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TDeployGroup, nRequestID++);
//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
// #pragma endregion

// #pragma region update_test
//     cout << "ManageUser update request, id=" << nRequestID << endl;
//     for (size_t i = 0; i < vecManage.size(); i++)
//     {
//         memset(vecManage[i].ManagerPasswd, 0, sizeof(vecManage[i].ManagerPasswd));
//         sprintf(vecManage[i].ManagerPasswd, "update_id%d", i);
//     }
//     dpimpl.ReqInsUpdDelByEletype<ManageUser>(vecManage, EElementType::TManageUser, nRequestID++, ECommandType::TUpdate);
//     cout << "ServerConfig update request, id=" << nRequestID << endl;
//     for (size_t i = 0; i < vecServer.size(); i++)
//     {
//         memset(vecServer[i].ServerIP, 0, sizeof(vecServer[i].ServerIP));
//         sprintf(vecServer[i].ServerIP, "update_id%d", i);
//     }
//     dpimpl.ReqInsUpdDelByEletype<ServerConfig>(vecServer, EElementType::TServerConfig, nRequestID++, ECommandType::TUpdate);
// #pragma endregion

// #pragma region qry_after_upd
//     filterVec.clear();
//     cout << "ManageUser query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TManageUser, nRequestID++);
//     cout << "ServerConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TServerConfig, nRequestID++);
//     cout << "StrategyConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TStrategyConfig, nRequestID++);
//     cout << "DeployConfig query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TDeployConfig, nRequestID++);
//     cout << "DeployGroup query request, id=" << nRequestID << endl;
//     dpimpl.ReqQryByColumnFilter(filterVec, EElementType::TDeployGroup, nRequestID++);
//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
// #pragma endregion

    std::this_thread::sleep_for(std::chrono::seconds(1200));

    dpimpl.CloseAsyncApi();
    return 0;
}