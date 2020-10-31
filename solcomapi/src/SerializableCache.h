#pragma once

#include "ISerializable.h"
#include "SillyCommand.h"

//ISerializable所有派生类对象缓存
class SerializableCache
{
private:
    //******这里包含所有ISerializable派生类成员******//
    HeartbeatCommand hbCmd;
    TestCommand tstCmd;
    //******这里包含所有ISerializable派生类成员*****//

public:
    SerializableCache(/* args */) {}
    ~SerializableCache() {}
    ISerializable *GetCachedObj(ERequestType objType)
    {
        switch (objType)
        {
        case ERequestType::TypeReq_Heartbeat:
            return &hbCmd;
        case ERequestType::TypeReq_TestCmd:
            return &tstCmd;
        default:
            return nullptr;
        }
    }
};
