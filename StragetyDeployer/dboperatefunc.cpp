#pragma once

#include <string.h>
#include <iostream>
#include "dboperatefunc.h"
#include "SQLiteDatabase.h"
#include "dbopmacros.h"

//query
vector<ManageUser> qryManageUserBySql(const string &sql)
{
    vector<ManageUser> res;
    QryBegin(sql);
    ManageUser user;
    while (resultSet.next())
    {
        memset(&user, 0, sizeof(user));
        user.ManagerID = resultSet.getInteger(0);
        strcpy(user.ManagerName, resultSet.getString(1).c_str());
        strcpy(user.ManagerPasswd, resultSet.getString(2).c_str());
        res.push_back(user);
    }
    QryEnd;
    return std::move(res);
}
vector<ServerConfig> qryServerConfigBySql(const string &sql)
{
    vector<ServerConfig> res;
    QryBegin(sql);
    ServerConfig cfg;
    while (resultSet.next())
    {
        memset(&cfg, 0, sizeof(cfg));
        strcpy(cfg.ServerName, resultSet.getString(0).c_str());
        strcpy(cfg.ServerIP, resultSet.getString(1).c_str());
        cfg.ServerPort = resultSet.getInteger(2);
        res.push_back(cfg);
    }
    QryEnd;
    return std::move(res);
}
vector<StrategyConfig> qryStrategyConfigBySql(const string &sql)
{
    vector<StrategyConfig> res;
    QryBegin(sql);
    StrategyConfig cfg;
    while (resultSet.next())
    {
        memset(&cfg, 0, sizeof(cfg));
        strcpy(cfg.ConfigName, resultSet.getString(0).c_str());
        strcpy(cfg.PlatformName, resultSet.getString(1).c_str());
        strcpy(cfg.StragetyName, resultSet.getString(2).c_str());
        strcpy(cfg.SrcRootDir, resultSet.getString(3).c_str());
        cfg.OwnerID = resultSet.getInteger(4);
        res.push_back(cfg);
    }
    QryEnd;
    return std::move(res);
}
vector<DeployConfig> qryDeployConfigBySql(const string &sql)
{
    vector<DeployConfig> res;
    QryBegin(sql);
    DeployConfig cfg;
    while (resultSet.next())
    {
        memset(&cfg, 0, sizeof(cfg));
        strcpy(cfg.ConfigName, resultSet.getString(0).c_str());
        strcpy(cfg.ServerName, resultSet.getString(1).c_str());
        strcpy(cfg.UserName, resultSet.getString(2).c_str());
        strcpy(cfg.UserPasswd, resultSet.getString(3).c_str());
        strcpy(cfg.DstRootDir, resultSet.getString(4).c_str());
        strcpy(cfg.ExeUser, resultSet.getString(5).c_str());
        cfg.OwnerID = resultSet.getInteger(6);
        res.push_back(cfg);
    }
    QryEnd;
    return std::move(res);
}
vector<DeployGroup> qryDeployGroupBySql(const string &sql)
{
    vector<DeployGroup> res;
    QryBegin(sql);
    DeployGroup cfg;
    while (resultSet.next())
    {
        memset(&cfg, 0, sizeof(cfg));
        strcpy(cfg.GroupName, resultSet.getString(0).c_str());
        strcpy(cfg.StragetyConfig, resultSet.getString(1).c_str());
        strcpy(cfg.DeployConfig, resultSet.getString(2).c_str());
        cfg.OwnerID = resultSet.getInteger(3);
        res.push_back(cfg);
    }
    QryEnd;
    return std::move(res);
}
//delete, return effected rows if success, else -1;
int delManagerUser(const vector<ManageUser> &objVec)
{
    string sql = "delete from ManageUser where ManagerID=?";
    DelByUniqueCol(sql, Integer, ManagerID, objVec);
}
int delServerConfig(const vector<ServerConfig> &objVec)
{
    string sql = "delete from ServerConfig where ServerName=?";
    DelByUniqueCol(sql, String, ServerName, objVec);
}
int delStrategyConfig(const vector<StrategyConfig> &objVec)
{
    string sql = "delete from StrategyConfig where ConfigName=?";
    DelByUniqueCol(sql, String, ConfigName, objVec);
}
int delDeployConfig(const vector<DeployConfig> &objVec)
{
    string sql = "delete from DeployConfig where ConfigName=?";
    DelByUniqueCol(sql, String, ConfigName, objVec);
}
int delDeployGroup(const vector<DeployGroup> &objVec)
{
    string sql = "delete from DeployGroup where GroupName=?";
    DelByUniqueCol(sql, String, GroupName, objVec);
}
//insert, return effected rows if success, else -1;
int insManagerUser(const vector<ManageUser> &objVec)
{
    string sql = "insert into ManageUser(ManagerID, ManagerName, ManagerPasswd) values(?,?,?)";
    InsertByVector(sql, objVec, Integer, ManagerID, String, ManagerName, String, ManagerPasswd);
}
int insServerConfig(const vector<ServerConfig> &objVec)
{
    string sql = "insert into ServerConfig(ServerName, ServerIP,ServerPort) values(?,?,?)";
    InsertByVector(sql, objVec, String, ServerName, String, ServerIP, Integer, ServerPort);
}
int insStrategyConfig(const vector<StrategyConfig> &objVec)
{
    string sql = "insert into StrategyConfig(ConfigName, PlatformName, StragetyName, SrcRootDir, OwnerID) values(?,?,?,?,?)";
    InsertByVector(sql, objVec, String, ConfigName, String, PlatformName, String, StragetyName, String, SrcRootDir, Integer, OwnerID);
}
int insDeployConfig(const vector<DeployConfig> &objVec)
{
    string sql = "insert into DeployConfig(ConfigName, ServerName, UserName, UserPasswd, DstRootDir, ExeUser, OwnerID) values(?,?,?,?,?,?,?)";
    InsertByVector(sql, objVec, String, ConfigName, String, ServerName, String, UserName, String, UserPasswd, String, DstRootDir, String, ExeUser, Integer, OwnerID);
}
int insDeployGroup(const vector<DeployGroup> &objVec)
{
    string sql = "insert into DeployConfig(GroupName, StragetyConfig, DeployConfig, OwnerID) values(?,?,?,?)";
    InsertByVector(sql, objVec, String, GroupName, String, StragetyConfig, String, DeployConfig, Integer, OwnerID);
}
//update, return effected rows if success, else -1;
int updManagerUser(const vector<ManageUser> &objVec)
{
    string sql = "update ManageUser set ManagerName=?, ManagerPasswd=? where ManagerID=?";
    UpdateByVector(sql, objVec, String, ManagerName, String, ManagerPasswd, Integer, ManagerID);
}
int updServerConfig(const vector<ServerConfig> &objVec)
{
    string sql = "update ServerConfig set ServerIP=?, ServerPort=? where  ServerName=?";
    UpdateByVector(sql, objVec, String, ServerIP, Integer, ServerPort, String, ServerName);
}
int updStrategyConfig(const vector<StrategyConfig> &objVec)
{
    string sql = "update StrategyConfig set PlatformName=?, StragetyName=?, SrcRootDir=?, OwnerID=? where ConfigName=?";
    UpdateByVector(sql, objVec, String, PlatformName, String, StragetyName, String, SrcRootDir, Integer, OwnerID, String, ConfigName, );
}
int updDeployConfig(const vector<DeployConfig> &objVec)
{
    string sql = "update DeployConfig set ServerName=?, UserName=?, UserPasswd=?, DstRootDir=?, ExeUser=?, OwnerID=? where ConfigName=?";
    UpdateByVector(sql, objVec, String, ServerName, String, UserName, String, UserPasswd, String, DstRootDir, String, ExeUser, Integer, OwnerID, String, ConfigName);
}
int updDeployGroup(const vector<DeployGroup> &objVec)
{
    string sql = "update DeployConfig set StragetyConfig=?, DeployConfig=?, OwnerID=? where GroupName=?";
    InsertByVector(sql, objVec, String, StragetyConfig, String, DeployConfig, Integer, OwnerID, String, GroupName);
}