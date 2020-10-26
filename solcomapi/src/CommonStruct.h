#pragma once

//包类型，req/ack
enum EPkgReqAckType
{
    Type_Req = 0, //请求
    Type_Ack,     //应答
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
    uint8_t repeatTimes; //重复发的次数
    uint8_t reqAckType;  //包类型
    uint16_t pkgBodyLen; //包体长度
    uint64_t serialID;   //包序列号，递增
};
#pragma pack(pop)

#define PackageHeadLen sizeof(PackageHeadField)
