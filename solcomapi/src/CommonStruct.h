#pragma once

//包类型，req/ack
enum EPkgReqRtnType
{
    Type_Req = 0, //请求
    Type_Rtn,     //推送
};
//接收状态
enum ERecvStatType
{
    Type_RecvHead = 0, //接收包头
    Type_RecvBody,     //接收包体
};

#pragma pack(push, 1)
struct PackageHeadField
{
    uint8_t versionID;   //版本号
    uint8_t reqRtnType;  //包类型
    uint16_t pkgBodyLen; //包体长度
    uint64_t requestID;  //请求序列号,如果是request请求
    PackageHeadField()
    {
        versionID = 1;
    }
};
#pragma pack(pop)

#define PackageHeadLen sizeof(PackageHeadField)

#define MAX_WRITE_IDLE_INTERVAL 3
#define MAX_IDLE_INTERVAL 100