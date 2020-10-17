#define PKG_BODY_DEFAULT_LENGTH 65535 //默认包体最大长度
#define MAX_IDLE_INTERVAL 20          //最大空闲时间(s)，无数据收发，开启心跳检测
#define MAX_TIMEOUT_INTERVAL 200      //最大超时时间(s)，无数据收发（包括心跳），断开连接

/////////////////////////////////通信包结构定义/////////////////////////////////////
enum PkgIDEnum
{
    PKG_NONE = 0x100, //空包，无意义
    PKG_CONNECT,      //连接
    PKG_LOGIN,        //登录
    PKG_LGOOUT,       //退出登录
    PKG_CHECKINFO,    //基础信息校验
    PKG_FILE,         //文件报文
    PKG_HEARTBEAT,    //心跳
};
//tcp 包头结构
struct PkgHeadField
{
    PkgIDEnum pkgID; //包类型
    int requestID;   //请求序列号
    bool isLast;     //请求末尾标志
    int pkgBodyLen;  //包体数据长度
};
#define PKG_HEAD_LENGTH sizeof(PkgHeadField)
//通信包结构
struct PkgStructField
{
    PkgHeadField pkgHead; //包头
    char *pkgData;        //正文数据，长度最大不超过PKG_DEFAULT_LENGTH
    PkgStructField()
    {
        pkgData = new char[PKG_BODY_DEFAULT_LENGTH];
    }
    ~PkgStructField()
    {
        delete[] pkgData;
    }
};
/////////////////////////////////通信包结构定义/////////////////////////////////////

//buffer
class BytesBuffer
{
public:
    int totalSize;    //byteBuffer总长度
    int usedSize;     //byteBuffer使用的字节数
    char *byteBuffer; //当前节点字节缓存块
    bool isBusy;      //忙标志
public:
    BytesBuffer()
        : usedSize(0), isBusy(false)
    {
        totalSize = PKG_BODY_DEFAULT_LENGTH + PKG_HEAD_LENGTH;
        byteBuffer = new char[totalSize];
    }
    ~BytesBuffer()
    {
        delete[] byteBuffer;
    }
    void clear()
    {
        isBusy = false;
        usedSize = 0;
    }
};

///////////////结构体定义////////////////////
//登录请求
struct LoginField
{
    char userId[64];
    char passWd[64];
};
//登出请求
struct LogoutField
{
    char userId[64];
};
//文件信息校验
struct FileCheckInfoField
{
    char filePathName[256]; //文件名，全路径名或相对默认目录的路径名
    char fileVerDate[16];   //文件版本日期,YYMMMMDD
    char ownUser[16];       //属主
    char ownGroup[16];      //属组
    int fileVerTime;        //文件版本时间,HHmmSSmsmsms,
    int fileSize;           //文件大小
};
//文件
struct FileStreamField
{
    char byteArray[61440]; //字节数组，最长一次发送60k
    int arrayLen;          //长度
};
//心跳检测
struct HeartbeatField
{
    char chrBeat; //0-req,1-ack
};
struct ConnectField
{
    char ipAddr[16];
    int nPort;
};

///////////////结构体定义////////////////////
