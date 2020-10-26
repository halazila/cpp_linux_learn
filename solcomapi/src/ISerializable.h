#pragma once

#include "CByteArray.h"

enum ERequestType
{
    TypeReq_None = 0,
    TypeReq_Connect,
    TypeReq_Heartbeat,
};

class ISerializable
{
public:
    uint8_t nType; //接口类型，ERequestType

public:
    ISerializable() : nType(0) {}
    virtual ~ISerializable() {}
    virtual void Serialize(CByteArray &dstByteArray)
    {
        dstByteArray.Append(nType);
    }
    virtual void Deserialize(CByteArray &srcByteArray)
    {
        srcByteArray.HeadIntercept(nType);
    }
    virtual int dataSize()
    {
        //size of non-static data member (member function not included),
        //attention to byte alignment
        return sizeof(*this);
    }
};
