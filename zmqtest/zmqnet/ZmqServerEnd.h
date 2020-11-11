#include "zmq.hpp"
#include <unordered_map>
#include <thread>

using namespace std;

struct ZmqClient
{
    string routerID; //router id
    string clientID; //client id
};

class ZmqServerEnd
{
private:
    //server data
    zmq::context_t zmqcontext;
    zmq::socket_t zmqsocket;
    unordered_map<string, ZmqClient *> mapClients; //<clientID, ZmqClient*>
    int nPort;
    bool bStop = false;
    thread pollThread;

public:
    ZmqServerEnd();
    ~ZmqServerEnd();
    void SetPort(int port);
    int Port();
    void Start();
    void Stop();
    void Join();
    int Send(const string &routerid, char *data, int len); //return length of bytes sended
    int ErrorNo();
    void MsgHandle(const string &routerid, const string &clientid, char *pdata, int nlen);

private:
    void pollFunction();
};
