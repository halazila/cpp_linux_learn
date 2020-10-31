#pragma once

#include <iostream>
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

class TestCommand : public ISerializable
{
private:
    char strData[32];
    double dbData;
    int intData;

public:
    TestCommand(/* args */)
    {
        nType = ERequestType::TypeReq_TestCmd;
        sprintf(strData, "%s", "Test Message");
        dbData = 10 + (double)rand() / RAND_MAX;
        intData = rand() % 100;
    }
    //首字节固定设为nType的值
    void Serialize(CByteArray &dstByteArray) override
    {
        ISerializable::Serialize(dstByteArray);
        dstByteArray.Append(strData).Append(dbData).Append(intData);
    }
    void Deserialize(CByteArray &srcByteArray) override
    {
        ISerializable::Deserialize(srcByteArray);
        srcByteArray.HeadIntercept(strData).HeadIntercept(dbData).HeadIntercept(intData);
    }
    void Print()
    {
        char strmsg[128] = {0};
        sprintf(strmsg, "TestCommand:: strData:%s, dbData:%f, intData:%d\n", strData, dbData, intData);
        std::cout << strmsg;
    }
    ~TestCommand() {}
};
