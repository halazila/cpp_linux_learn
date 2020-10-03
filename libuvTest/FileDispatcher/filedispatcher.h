#include "uv.h"

#define MAX_UNREPLY_HEARTBEAT 15                //最大未回复心跳数，保活机制
#define PKG_PROTOCOL_LEN 16                     //协议头字符长度（包含'\0'）
#define PKG_PROTOCOL_IDENTITY "##FILEDISPATCH#" //协议头，做校验用
#define PKG_DEFAULT_LENGTH 65535                //默认包体最大长度

//应用错误宏定义
#define DISPATCHER_OK 0
#define DISPATCHER_ERR -1

//解析包相关宏定义
#define PARSE_PACKAGE_ERR -1 //解析包出错
#define PARSE_PACKAGE_NONE 0 //没有包可解析
#define PARSE_PACKAGE_PART 1 //只解析部分数据
#define PARSE_PACKAGE_OK 2   //解析到完整包

enum PackageIDEnum
{
    PKG_LOGIN = 0x0101, //登录请求
    PKG_LGOOUT,         //退出登录请求
    PKG_CHECKINFO,      //基础信息校验请求
    PKG_HEARTBEAT,      //心跳请求
    PKG_WRITEFILE,      //普通模式发送文件，普通报文格式，文件报文包含包头结构
    PKG_READFILE,       //普通模式接收文件，普通报文格式，文件报文包含包头结构
    PKG_SENDFILE,       //零拷贝发送文件请求，文件报文为纯文件内容
    PKG_RECVFILE,       //零拷贝接收文件请求，文件报文为纯文件内容
};

//tcp 包头结构
struct PkgHeadField
{
    char pkgProto[PKG_PROTOCOL_LEN]; //协议号，对校验不通过的数据包直接丢弃，并断开连接
    PackageIDEnum pkgID;             //包类型
    int pkgSerialID;                 //包序列号，从0开始
    int pkgNums;                     //当前消息包总数
    int pkgBodyLen;                  //包体数据长度
};
#define PKG_HEAD_LENGTH sizeof(PkgHeadField)

//通信包结构
struct PkgStructField
{
    PkgHeadField pkgHead;             //包头
    char pkgData[PKG_DEFAULT_LENGTH]; //正文数据，长度最大不超过PKG_DEFAULT_LENGTH
    PkgStructField()
    {
    }
    ~PkgStructField()
    {
    }
};

struct PkgReqLoginField
{
    char userName[32];     //用户名
    char userPass[32];     //用户密码
    char defaultPath[128]; //默认上传路径
};

struct PkgReqCheckFileField
{
    char filePathName[256]; //文件名，全路径名或相对默认目录的路径名
    char fileVerDate[16];   //文件版本日期,YYMMMMDD
    int fileVerTime;        //文件版本时间,HHmmSSmsmsms,
    int fileSize;           //文件大小
};

struct PkgRspCheckFileField
{
    char filePathName[256]; //文件名，全路径名或相对默认目录的路径名
    int checkValue;         //0-client端文件比server端文件新，无需更新，1-全部更新，2-部分更新，从fileOffset开始
    int fileOffset;         //部分更新时，文件更新偏移量
};

typedef struct
{
    char filePathName[256]; //文件名，全路径名或相对默认目录的路径名
    int fileMode;           //文件模式
    int fileOffset;         //文件偏移量
    int fileSize;           //文件大小
} PkgSendFileFiled, PkgRecvFileField;

//接收模式
enum RecvModeEnum
{
    MODE_RECV_NONE, //正常接收模式，未接收到包头
    MODE_RECV_HEAD, //正常接收模式，已经接收到包头
    MODE_RECV_BODY, //正常接收模式，已经接收到包体
    MODE_READ_FILE, //正常接收模式，接收文件
    MODE_RECV_FILE, //接收纯文件包模式
};

//接收状态
struct RecvStatusField
{
    RecvModeEnum recvMode;              //接收模式
    int recvdSize;                      //已接收数据字节数
    int remainSize;                     //剩余需要接收（读）的字节数
    char recvdData[PKG_DEFAULT_LENGTH]; //已接收数据缓存，最长不超过PKG_DEFAULT_LENGTH
    RecvStatusField()
    {
        recvMode = MODE_RECV_NONE;
        recvdSize = 0;
        remainSize = PKG_HEAD_LENGTH;
    }
    ~RecvStatusField()
    {
    }
    void ResetStatus()
    {
        recvMode = MODE_RECV_NONE;
        recvdSize = 0;
        remainSize = PKG_HEAD_LENGTH;
    }
};

//发送模式
enum SendModeEnum
{
    MODE_WRITE_MSG,  //正常发送小心模式
    MODE_WRITE_FILE, //正常发送文件模式
    MODE_SEND_FILE,  //发送纯文件包模式
};

struct SendStatusField
{
    SendModeEnum sendMode;             //发送模式
    int sendSize;                      //已发送字节数
    int remainSize;                    //剩下需要发送的字节数
    char sendData[PKG_DEFAULT_LENGTH]; //要发送的字节数缓存
};

//接收端文件接收状态，文件接收端缓存，断网重连后续传
struct RecvFileStatusField
{
    char filePathName[256]; //文件名，全路径名或相对默认目录的路径名
    char fileVerDate[16];   //文件版本日期,YYMMMMDD
    int fileVerTime;        //文件版本时间,HHmmSSmsmsms
    int recvdBytes;         //接收成功的字节数
    int totalBytes;         //需要接收的总字节数
};

struct BytesLinkedBuf
{
    int startPos;            //开始有效位置，在此位置之前的字符无效
    int usedSize;            //有效字节数，byteBuffer超出（下标大于等于）startPos+usedSize的部分都是无效字节
    char *byteBuffer;        //当前节点字节缓存块
    BytesLinkedBuf *ptrNext; //指向下一个节点
    BytesLinkedBuf()
    {
        startPos = 0;
        usedSize = 0;
        byteBuffer = nullptr;
        ptrNext = nullptr;
    }
    ~BytesLinkedBuf()
    {
        if (byteBuffer)
        {
            delete[] byteBuffer;
        }
        if (ptrNext)
        {
            delete ptrNext;
        }
    }
    void Realse()
    {
        delete[] byteBuffer;
        byteBuffer = nullptr;
        startPos = usedSize = 0;
        ptrNext = nullptr;
    }
};

struct ServerConn
{
    uv_tcp_t *tcpHandle; //tcp handle
    char ipAddr[16];     //服务端IPv4地址
    int port;            //服务端端口
    int recvBytes;       //接收成功的字节数
    int totalBytes;      //需要接收的总字节数
    int pkgDataLen;      //当下消息的数据长度，一般是读到包头后设置该值，下次读取该长度的正文数据
};

typedef PackageIDEnum CmdTypeEnum;

//服务端处理命令结构体
struct CommondStructField
{
    CmdTypeEnum cmdTYpe;              //命令类型
    char cmdData[PKG_DEFAULT_LENGTH]; //命令内容，最长不超过PKG_DEFAULT_LENGTH
    bool bLast;
    CommondStructField()
    {
        bLast = true;
    }
    ~CommondStructField()
    {
    }
};