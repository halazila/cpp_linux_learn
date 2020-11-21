#pragma pack(push, 4)
/////数据库表元素类型-begin/////
//管理用户
struct ManageUser
{
    int ManagerID;
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
    int OwnerID;
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
    int OwnerID;
};
//部署组
struct DeployGroup
{
    char GroupName[64];
    char StragetyConfig[64];
    char DeployConfig[64];
    int OwnerID;
};
/////数据库表元素类型-end/////
//命令类型
enum EManipulateType : int
{
    TConnect,
    TSelect,
    TInsert,
    TDelete,
    TUpdate,
    TDeploy,
    TExecute,
    TFinish,
};


#pragma pack(pop)
