
#pragma once

#pragma pack(push, 4)
/////数据库表元素-begin/////
//管理用户
struct ManageUser
{
    char ManagerName[64];
    char ManagerPasswd[64];
};
//服务器配置
struct ServerConfig
{
    char ServerName[64];
    char ServerIP[64];
    int ServerPort;
};
//策略文件配置
struct StrategyConfig
{
    char ConfigName[64];
    char PlatformName[128];
    char StragetyName[128];
    char SrcRootDir[256];
    char ManagerName[64]; //ManageUser.ManagerName
};
//部署配置
struct DeployConfig
{
    char ConfigName[64];
    char ServerName[64];
    char UserName[64];
    char UserPasswd[64];
    char DstRootDir[256];
    char ExeUser[64];
    char ManagerName[64]; //ManageUser.ManagerName
};
//部署组
struct DeployGroup
{
    char GroupName[64];
    char StragetyConfig[64];
    char DeployConfig[64];
    char ManagerName[64]; //ManageUser.ManagerName
};
/////数据库表元素-end/////
//返回消息基类
struct CommResponse
{
    int ErrorID;
    char ErrMsg[64];
};
//请求返回消息，api向服务端请求
struct ReqResponse : public CommResponse
{
    int RequestID;
    int CmdType;
};
#pragma pack(pop)
//命令类型枚举值
enum ECommandType : int
{
    TNullCmd = 0,
    TLogin,
    TLogout,
    TKeepAlive,
    TQuery,
    TInsert,
    TDelete,   //删除对象
    TDelByCol, //按筛选条件删除
    TUpdate,
    TDeploy,
    TExecute,
    TFinish,
};
//元数据类型枚举值
enum EElementType : int
{
    TManageUser = 0,
    TServerConfig,
    TStrategyConfig,
    TDeployConfig,
    TDeployGroup,
};
//server to client message pattern
enum STCMsgPattern : int
{
    TPassiveResponse, //被动应答
    TActivePush,      //主动推送
};
//ReqResponse.ErrorID 类型
enum EResponseErrType : int
{
    TSuccess,
    TReplicatedLogin, //重复登录
    TIdentifyErr,
    TNoLogin, //未登录
    TMultiLogin,
    TDbError,
    TSshError,
};
//消息帧接收状态
enum EMsgRecvState : int
{
    StatInit = 0,
    StatGetPattern,
    StatGetCmd,
    StatGetRsp,
    StatGetEleType,
};
//表列筛选条件
struct ColumnFilter
{
    std::string ColName; //列名
    std::string ColVal;  //列值
};

////消息帧顺序定义///
//**client-->server**// 除keep-alive命令以外其他都有requestid
//managerid->cmd->[requestid->[element type]->element...]
//**server-->client**//
//PassiveResponse->Response->[element-type->element...]
//     ActivePush->cmd-type->[element-type->element...]

////特殊变量声明
//表名映射

class StaticDefines
{
public:
    thread_local static std::string sqlite_error_msg;
    static const char *AllTableNames[];

public:
    StaticDefines(/* args */) {}
    ~StaticDefines() {}
};
