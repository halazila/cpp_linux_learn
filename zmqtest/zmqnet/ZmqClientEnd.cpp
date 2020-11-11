#include "ZmqClientEnd.h"

ZmqClientEnd::ZmqClientEnd(/* args */)
{
    zmqcontext = zmq::context_t(1);
    zmqsocket = zmq::socket_t(zmqcontext, ZMQ_DEALER);
}

ZmqClientEnd::~ZmqClientEnd()
{
    zmqsocket.close();
    zmqcontext.close();
}

void ZmqClientEnd::SetClientID(const std::string &strid)
{
    clientID = strid;
}

void ZmqClientEnd::Connect(const std::string &serverAddr)
{
    std::string str = "tcp://" + serverAddr;
    zmqsocket.connect(str.c_str());
}

int ZmqClientEnd::Send(char *data, int len)
{
    int nRet;
    zmq_send(zmqsocket, clientID.c_str(), clientID.length(), ZMQ_SNDMORE);
    nRet = zmq_send(zmqsocket, data, len, 0);
    return nRet;
}

int ZmqClientEnd::Receive(char *data, int nlen)
{
    int nRet;
    nRet = zmq_recv(zmqsocket, data, nlen, 0);
    return nRet;
}
