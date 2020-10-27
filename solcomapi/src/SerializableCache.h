#pragma once

#include "ISerializable.h"
#include "SillyCommand.h"

//ISerializable所有派生类对象缓存
class SerializableCache
{
private:
    HeartbeatCommand hbCmd;

public:
    SerializableCache(/* args */) {}
    ~SerializableCache() {}
    ISerializable *GetCachedObj(ERequestType objType)
    {
        switch (objType)
        {
        case ERequestType::TypeReq_Heartbeat:
            return &hbCmd;
            break;
        default:
            return nullptr;
            break;
        }
    }
};
