#pragma once

#include "CByteArray.h"

enum ERequestType
{
    TypeReq_None = 0,
    TypeReq_Heartbeat,
};

class ISerializable
{
public:
    uint8_t nType; //接口类型，ERequestType

public:
    ISerializable() : nType(0) {}
    virtual ~ISerializable() {}
    //首字节固定设为nType的值
    virtual void Serialize(CByteArray &dstByteArray)
    {
        dstByteArray.Append(nType);
    }
    virtual void Deserialize(CByteArray &srcByteArray)
    {
        srcByteArray.HeadIntercept(nType);
    }
    //返回一个大于成员变量内存大小的非精确值
    virtual int dataSize()
    {
        //size of non-static data member (member function not included),
        //attention to byte alignment
        return sizeof(*this);
    }
};
