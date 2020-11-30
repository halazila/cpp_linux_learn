#include <vector>
#include "SQLiteDatabase.h"
#include "CommonStruct.h"

using namespace std;

#define DISPATCHER_DATABASE "ATDConfig.db"

//query
vector<ManageUser> qryManageUserBySql(const string &sql);
vector<ServerConfig> qryServerConfigBySql(const string &sql);
vector<StrategyConfig> qryStrategyConfigBySql(const string &sql);
vector<DeployConfig> qryDeployConfigBySql(const string &sql);
vector<DeployGroup> qryDeployGroupBySql(const string &sql);
//delete, return effected rows if success, else -1;
int delManagerUser(const vector<ManageUser> &objVec);
int delServerConfig(const vector<ServerConfig> &objVec);
int delStrategyConfig(const vector<StrategyConfig> &objVec);
int delDeployConfig(const vector<DeployConfig> &objVec);
int delDeployGroup(const vector<DeployGroup> &objVec);
//insert, return effected rows if success, else -1;
int insManagerUser(const vector<ManageUser> &objVec);
int insServerConfig(const vector<ServerConfig> &objVec);
int insStrategyConfig(const vector<StrategyConfig> &objVec);
int insDeployConfig(const vector<DeployConfig> &objVec);
int insDeployGroup(const vector<DeployGroup> &objVec);
//update, return effected rows if success, else -1;
int updManagerUser(const vector<ManageUser> &objVec);
int updServerConfig(const vector<ServerConfig> &objVec);
int updStrategyConfig(const vector<StrategyConfig> &objVec);
int updDeployConfig(const vector<DeployConfig> &objVec);
int updDeployGroup(const vector<DeployGroup> &objVec);