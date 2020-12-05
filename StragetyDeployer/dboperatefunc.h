#pragma once

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
//delete by sql
int delBySql(const string &sql);
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

/////////////数据库操作模板函数////////////////
///////////////模板 + 特化////////////////////
////query////
template <class T>
vector<T> qryDataBySql(const string &sql)
{
    return vector<T>();
}
template <>
inline vector<ManageUser> qryDataBySql(const string &sql)
{
    return std::move(qryManageUserBySql(sql));
}
template <>
inline vector<ServerConfig> qryDataBySql(const string &sql)
{
    return std::move(qryServerConfigBySql(sql));
}
template <>
inline vector<StrategyConfig> qryDataBySql(const string &sql)
{
    return std::move(qryStrategyConfigBySql(sql));
}
template <>
inline vector<DeployConfig> qryDataBySql(const string &sql)
{
    return std::move(qryDeployConfigBySql(sql));
}
template <>
inline vector<DeployGroup> qryDataBySql(const string &sql)
{
    return std::move(qryDeployGroupBySql(sql));
}
////delete/////
template <class T>
int delByObjects(const vector<T> &objVec)
{
    return 0;
}
template <>
inline int delByObjects<ManageUser>(const vector<ManageUser> &objVec)
{
    return delManagerUser(objVec);
}
template <>
inline int delByObjects<ServerConfig>(const vector<ServerConfig> &objVec)
{
    return delServerConfig(objVec);
}
template <>
inline int delByObjects<StrategyConfig>(const vector<StrategyConfig> &objVec)
{
    return delStrategyConfig(objVec);
}
template <>
inline int delByObjects<DeployConfig>(const vector<DeployConfig> &objVec)
{
    return delDeployConfig(objVec);
}
template <>
inline int delByObjects<DeployGroup>(const vector<DeployGroup> &objVec)
{
    return delDeployGroup(objVec);
}
//////insert////
template <class T>
int insByObjects(const vector<T> &objVec)
{
    return 0;
}
template <>
inline int insByObjects<ManageUser>(const vector<ManageUser> &objVec)
{
    return insManagerUser(objVec);
}
template <>
inline int insByObjects<ServerConfig>(const vector<ServerConfig> &objVec)
{
    return insServerConfig(objVec);
}
template <>
inline int insByObjects<StrategyConfig>(const vector<StrategyConfig> &objVec)
{
    return insStrategyConfig(objVec);
}
template <>
inline int insByObjects<DeployConfig>(const vector<DeployConfig> &objVec)
{
    return insDeployConfig(objVec);
}
template <>
inline int insByObjects<DeployGroup>(const vector<DeployGroup> &objVec)
{
    return insDeployGroup(objVec);
}
//////update////
template <class T>
int updByObjects(const vector<T> &objVec)
{
    return 0;
}
template <>
inline int updByObjects<ManageUser>(const vector<ManageUser> &objVec)
{
    return updManagerUser(objVec);
}
template <>
inline int updByObjects<ServerConfig>(const vector<ServerConfig> &objVec)
{
    return updServerConfig(objVec);
}
template <>
inline int updByObjects<StrategyConfig>(const vector<StrategyConfig> &objVec)
{
    return updStrategyConfig(objVec);
}
template <>
inline int updByObjects<DeployConfig>(const vector<DeployConfig> &objVec)
{
    return updDeployConfig(objVec);
}
template <>
inline int updByObjects<DeployGroup>(const vector<DeployGroup> &objVec)
{
    return updDeployGroup(objVec);
}