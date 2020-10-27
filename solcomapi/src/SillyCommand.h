#pragma once

#include "ISerializable.h"

class HeartbeatCommand : public ISerializable
{
private:
    /* data */
public:
    HeartbeatCommand(/* args */)
    {
        nType = ERequestType::TypeReq_Heartbeat;
    }
    ~HeartbeatCommand() {}
};
