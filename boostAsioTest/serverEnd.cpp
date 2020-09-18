#include <iostream>
#include <functional>
#include <atomic>
#include <set>
#include <mutex>
#include <map>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

std::recursive_mutex printMutex;

void threadSafePrint(char *str)
{
    std::lock_guard<decltype(printMutex)> lock(printMutex);
    std::cout << str << std::endl;
}

class Client
{
public:
    boost::asio::ip::tcp::socket sock;
    int nHbtCount;
    int nId;

public:
    Client(boost::asio::io_service &ioServ, int id)
        : sock(ioServ), nHbtCount(0), nId(id)
    {
    }
    ~Client() {}
};

class ServerEnd
{

private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::endpoint endPoint;
    boost::asio::ip::tcp::acceptor acept;
    std::vector<boost::asio::ip::tcp::socket> clientSocks;
    std::map<int, std::shared_ptr<Client>> clientMap;
    int nMaxCliId;
    std::set<int> avaIdSet; //available clientId id
    boost::array<char, 1024> readBuffer;
    std::shared_ptr<boost::asio::deadline_timer> timerPtr;

private:
    int createClientId();
    void asyncAccept();
    void asyncDeadlineHeartBeat();

public:
    ServerEnd(const boost::asio::ip::tcp::endpoint &ep)
        : endPoint(ep), acept(ioService, endPoint), nMaxCliId(0), timerPtr(nullptr)
    {
    }
    ~ServerEnd() {}
    void operator()()
    {
        acept.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acept.listen();
        asyncAccept();
        asyncDeadlineHeartBeat();
        ioService.run();
    }
    void deadlineTimerHandle();
    void acceptHandler(const boost::system::error_code &err, std::shared_ptr<Client> clientPtr);
    void readHandler(const boost::system::error_code &err, int nbytes, std::shared_ptr<Client> clientPtr);
    void writeHandler(const boost::system::error_code &err, int nbytes, std::shared_ptr<Client> clientPtr);
};

int ServerEnd::createClientId()
{
    int id;
    if (avaIdSet.empty())
        id = ++nMaxCliId;
    else
    {
        auto it = avaIdSet.begin();
        id = *it;
        avaIdSet.erase(it);
    }
    return id;
}

void ServerEnd::asyncAccept()
{
    int id = createClientId();
    std::shared_ptr<Client> clientPtr(new Client(ioService, id));
    char str[64];
    sprintf(str, "Try Accept %d", id);
    threadSafePrint(str);
    acept.async_accept(clientPtr->sock, std::bind(&ServerEnd::acceptHandler, this, std::placeholders::_1, clientPtr));
}

void ServerEnd::asyncDeadlineHeartBeat()
{
    //send heart beat
    timerPtr.reset(new boost::asio::deadline_timer(ioService, boost::posix_time::seconds(2)));
    timerPtr->async_wait(std::bind(&ServerEnd::deadlineTimerHandle, this));
}

void ServerEnd::deadlineTimerHandle()
{
    for (auto it = clientMap.begin(); it != clientMap.end(); it++)
    {
        if (it->second.get() != nullptr)
        {
            if (++it->second->nHbtCount > 5) //send more than 5 heart beat msg since last receving heart beat msg
            {
                char str[128];
                sprintf(str, "line %d, lose connection after 5 heart beat\n", __LINE__);
                std::cout << str << std::endl;
                it->second->sock.close();
                avaIdSet.insert(it->second->nId);
                it->second.reset();
            }
            else
            {
                it->second->sock.async_write_some(boost::asio::buffer("Heart Beat"), std::bind(&ServerEnd::writeHandler, this, std::placeholders::_1, std::placeholders::_2, it->second));
            }
        }
    }
    asyncDeadlineHeartBeat();
}

void ServerEnd::acceptHandler(const boost::system::error_code &err, std::shared_ptr<Client> clientPtr)
{
    if (err)
    {
        std::cout << "ErrorCode:" << err.value() << "; ErrorMsg:" << err.message() << std::endl;
        avaIdSet.insert(clientPtr->nId);
        clientMap.erase(clientPtr->nId);
        return;
    }
    clientMap.insert(std::make_pair(clientPtr->nId, clientPtr));
    //output client info
    std::cout << "accept client: " << clientPtr->sock.remote_endpoint().address() << ":" << clientPtr->sock.remote_endpoint().port() << std::endl;
    clientPtr->sock.async_read_some(boost::asio::buffer(readBuffer), std::bind(&ServerEnd::readHandler, this, std::placeholders::_1, std::placeholders::_2, clientPtr));
    //start a new accept round
    asyncAccept();
}

void ServerEnd::readHandler(const boost::system::error_code &err, int nbytes, std::shared_ptr<Client> clientPtr)
{
    if (!err)
    {
        if (nbytes > 0)
        {
            std::cout << "Receive From Client " << clientPtr->sock.remote_endpoint().address().to_string() << ":" << clientPtr->sock.remote_endpoint().port() << "; " << readBuffer.c_array() << std::endl;
            if (strcmp(readBuffer.c_array(), "Heart Beat") == 0)
            {
                clientPtr->nHbtCount = 0;
            }
        }
        else
        {
            ///
        }
    }
    else if (err.value() != boost::system::errc::errc_t::interrupted)
    {
        char str[128];
        sprintf(str, "line %d, read error, errid: %d, errmsg: %s, will be closed.\n", __LINE__,
                err.value(), err.message().c_str());
        std::cout << str << std::endl;
        clientPtr->sock.close();
        avaIdSet.insert(clientPtr->nId); //recycle client id
    }
}

void ServerEnd::writeHandler(const boost::system::error_code &err, int nbytes, std::shared_ptr<Client> clientPtr)
{
    if (!err)
    {
        clientPtr->sock.async_read_some(boost::asio::buffer(readBuffer), std::bind(&ServerEnd::readHandler, this, std::placeholders::_1, std::placeholders::_2, clientPtr));
    }
    else if (err.value() == boost::system::errc::errc_t::broken_pipe)
    {
        char str[128];
        sprintf(str, "line %d, Remote Endpoint is closed.\n", __LINE__);
        std::cout << str << std::endl;
        clientPtr->sock.close();
        avaIdSet.insert(clientPtr->nId);
        return;
    }
}

int main(int argc, char *argv[])
{
    char ipAddr[64] = "0.0.0.0";
    int nPort = 8008;
    if (argc > 1)
        sprintf(ipAddr, "%s", argv[1]);
    if (argc > 2)
        nPort = atoi(argv[2]);
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ipAddr), nPort);
    ServerEnd servEnd(ep);
    servEnd();
    return 0;
}