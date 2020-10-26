#pragma once

#include "ISerializable.h"

class ConnectCommand : public ISerializable
{
private:
    /* data */
public:
    ConnectCommand(/* args */)
    {
        nType = ERequestType::TypeReq_Connect;
    }
    ~ConnectCommand() {}
};

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
