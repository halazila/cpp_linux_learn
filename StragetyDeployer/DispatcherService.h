#include <unordered_map>
#include <chrono>
#include "zmq.hpp"

#define MAX_SIMUL_ONLINE 5 //客户端同时在线最大数

class DispatcherClient
{
private:
    std::string m_strRouter;
    std::string m_strIdentify;
    //alive 探测
    std::chrono::steady_clock m_tLastWrite;
    std::chrono::steady_clock m_tLastRead;

public:
    DispatcherClient();
    ~DispatcherClient();

    friend class DispatcherService;
};

class DispatcherService
{
private:
    zmq::context_t m_ctx;
    zmq::socket_t m_sockBind;
    zmq::socket_t m_sockInprocServer;
    zmq::socket_t m_sockInprocClient;
    int m_nBindPort;
    std::unordered_map<std::string, std::shared_ptr<DispatcherClient>> m_mapClients;
    std::unordered_map<std::string, int> m_mapOnlineCns; //客户端多点同时在线统计

public:
    DispatcherService();
    ~DispatcherService();
    void SetPort(int port) { m_nBindPort = port; }
    void StartService() { pollFunc(); }

private:
    void pollFunc();
    void sendMsg(char *data, int len, bool bMore = false);
};
