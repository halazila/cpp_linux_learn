#pragma once

#include "ISerializable.h"
#include "SillyCommand.h"

class SerializableFactory
{
private:
    /* data */
public:
    SerializableFactory(/* args */) {}
    ~SerializableFactory() {}
    static ISerializable *CreateObject(ERequestType objType)
    {
        switch (objType)
        {
        case ERequestType::TypeReq_Heartbeat:
            return new HeartbeatCommand();
        case ERequestType::TypeReq_TestCmd:
            return new TestCommand();
        default:
            return nullptr;
        }
    }
};
