#include "uv.h"

#define MAX_UNREPLY_HEARTBEAT 15
#define MAX_PACKAGE_LENGTH 65535
#define PKG_PROTOCOL_LEN 32
#define PKG_PROTOCOL_IDENTITY "###CENTRALFILEDISPATCHER###"

#define DISPATCHER_OK 0
#define DISPATCHER_ERR -1

#define DEFAULT_FILE_CAP 16

#define PARSE_PACKAGE_ERR -1

enum PackageID
{
    PKG_LOGIN = 0x0101, //登录请求
    PKG_CHECKINFO,      //基础信息校验请求
    PKG_HEARTBEAT,      //心跳请求
    PKG_SENDFILE,       //发送文件请求
    PKG_RECVFILE,       //接收文件请求
};

//tcp 包头结构
struct PkgHead
{
    char pkgProto[PKG_PROTOCOL_LEN]; //协议号，对校验不通过的数据包直接丢弃，并断开连接
    int pkgID;                       //包类型
    int pkgSerialID;                 //包序列号，从0开始
    int pkgNum;                      //当前消息包总数
    int pkgBodyLen;                  //包体数据长度
};

//通信包结构
struct PkgStruct
{
    PkgHead pkgHead; //包头
    void *pkgData;   //正文数据
    PkgStruct()
    {
        pkgData = nullptr;
    }
    ~PkgStruct()
    {
        delete pkgData;
    }
};

#define PKG_HEAD_LENGTH sizeof(PkgHead)

struct PkgReqLogin
{
    char userName[32];     //用户名
    char userPass[32];     //用户密码
    char defaultPath[256]; //默认上传路径
};

struct PkgReqCheckFile
{
    char dstFilePathName[256]; //文件名，全路径名或相对上传目录的路径名
    char fileVerDate[16];      //文件版本日期,YYMMMMDD
    int fileVerTime;           //文件版本时间,HHmmSSmsmsms,
    int fileSize;              //文件大小
};

struct PkgRspCheckFile
{
    char dstFilePathName[256]; //文件名，全路径名或相对上传目录的路径名
    int checkValue;            //0-client端文件比server端文件新，无需更新，1-全部更新，2-部分更新，从fileOffset开始
    int fileOffset;            //部分更新时，文件更新偏移量
};

struct PkgSendFile
{
    char dstFilePathName[256]; //文件名，全路径名或相对上传目录的路径名
    int fileOffset;            //文件偏移量
    int fileSize;              //文件大小
};

struct FileInfo
{
    char filePathName[256]; //文件名，全路径名或相对上传目录的路径名
    char fileVerDate[16];   //文件版本日期,YYMMMMDD
    int fileVerTime;        //文件版本时间,HHmmSSmsmsms
    int sendBytes;          //发送成功的字节数
    int totalBytes;         //需要发送的总字节数
};

struct BytesLinkedBuf
{
    int usedSize;            //使用字节数，byteBuffer超出（下标大于等于）usedSize的部分都是无效字节
    char *byteBuffer;        //当前节点字节缓存块
    BytesLinkedBuf *ptrNext; //指向下一个节点
    BytesLinkedBuf()
    {
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
