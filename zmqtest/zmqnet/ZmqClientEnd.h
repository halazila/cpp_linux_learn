#include "zmq.hpp"
#include <string>

class ZmqClientEnd
{
private:
    zmq::context_t zmqcontext;
    zmq::socket_t zmqsocket;
    std::string clientID; //client id

public:
    ZmqClientEnd();
    ~ZmqClientEnd();
    void SetClientID(const std::string &strid);
    //serverAddr format, 127.0.0.1:8899
    void Connect(const std::string &serverAddr);
    int Send(char *data, int len);
    int Receive(char *data, int nlen);
};
