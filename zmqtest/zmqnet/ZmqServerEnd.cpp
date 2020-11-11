#include <functional>
#include <iostream>
#include "ZmqServerEnd.h"

ZmqServerEnd::ZmqServerEnd()
    : nPort(57900)
{
    //zmq socket
    zmqcontext = zmq::context_t(1);
    zmqsocket = zmq::socket_t(zmqcontext, ZMQ_ROUTER);
}

ZmqServerEnd::~ZmqServerEnd()
{
    zmqsocket.close();
    zmqcontext.close();
    if (pollThread.joinable())
        pollThread.join();
    for (auto it = mapClients.begin(); it != mapClients.end(); it++)
    {
        delete it->second;
    }
    mapClients.clear();
}

void ZmqServerEnd::SetPort(int port)
{
    nPort = port;
}

int ZmqServerEnd::Port()
{
    return nPort;
}

void ZmqServerEnd::Start()
{
    bStop = false;
    char addr[64] = {0};
    sprintf(addr, "tcp://*:%d", nPort);
    zmq_bind(zmqsocket, addr);
    pollThread = thread(bind(&ZmqServerEnd::pollFunction, this));
}

void ZmqServerEnd::Stop()
{
    bStop = true;
}

void ZmqServerEnd::Join()
{
    if (pollThread.joinable())
        pollThread.join();
}

int ZmqServerEnd::Send(const string &routerid, char *data, int len)
{
    int nRet = 0;
    //client address first
    zmq_send(zmqsocket, routerid.c_str(), routerid.length(), ZMQ_SNDMORE);
    //message sendond
    nRet = zmq_send(zmqsocket, data, len, 0);
    return nRet;
}

int ZmqServerEnd::ErrorNo()
{
    return zmq_errno();
}

void ZmqServerEnd::MsgHandle(const string &routerid, const string &clientid, char *pdata, int nlen)
{
    std::cout << string(pdata, nlen) << std::endl;

    ZmqClient *client = nullptr;
    auto it = mapClients.find(clientid);
    if (it == mapClients.end())
    {
        client = new ZmqClient;
        client->clientID = clientid;
        client->routerID = routerid;
    }
    else
    {
        client = it->second;
    }
    Send(routerid, pdata, nlen);
}

void ZmqServerEnd::pollFunction()
{
    char *recvarr = new char[65536];
    memset(recvarr, 0, 65536);
    zmq::pollitem_t items[] = {
        {zmqsocket, 0, ZMQ_POLLIN, 0}};

    while (!bStop)
    {
        zmq::poll(items, 1, 1000);
        if (items[0].revents & ZMQ_POLLIN)
        {
            int nrecv;
            //router id, ROUTER socket prefix
            nrecv = zmq_recv(zmqsocket, recvarr, 65536, 0); //zmq_recv
            if (nrecv < 0)
                continue;
            string routerid(recvarr, nrecv);
            //client id, message defined prefix
            nrecv = zmq_recv(zmqsocket, recvarr, 65536, 0); //zmq_recv
            if (nrecv < 0)
                continue;
            string clientid(recvarr, nrecv);
            //message
            nrecv = zmq_recv(zmqsocket, recvarr, 65536, 0);
            if (nrecv < 0)
                continue;
            MsgHandle(routerid, clientid, recvarr, nrecv);
        }
    }
    delete[] recvarr;
}
