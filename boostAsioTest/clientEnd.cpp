#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/array.hpp>

std::recursive_mutex printMutex;

void threadSafePrint(char *str)
{
    std::lock_guard<decltype(printMutex)> lock(printMutex);
    std::cout << str << std::endl;
}

class ClientEnd
{
private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::socket sock;
    boost::asio::ip::tcp::endpoint endPoint; //server end point
    boost::array<char, 1024> readBuffer;
    int nHbtCount = 0;

private:
    void readHandler(const boost::system::error_code &err, std::size_t bytes) //先不考虑粘包问题
    {
        if (!err)
        {
            if (bytes > 0)
            {
                char str[128];
                sprintf(str, "Receive Message: %s, Local Port: %d", readBuffer.c_array(), sock.local_endpoint().port());
                threadSafePrint(str);
                if (strcmp(readBuffer.c_array(), "Heart Beat") == 0)
                {
                    sock.async_write_some(boost::asio::buffer("Heart Beat"), std::bind(&ClientEnd::writeHandler, this, std::placeholders::_1, std::placeholders::_2));
                }
            }
            else
            {
                std::cout << "read 0 bytes" << std::endl;
            }
        }
        else if (err.value() != boost::system::errc::errc_t::interrupted)
        {
            char str[128];
            sprintf(str, "line %d; Read Error, id: %d, message: %s", __LINE__, err.value(), err.message().c_str());
            threadSafePrint(str);
            sock.close();
        }
    }
    void writeHandler(const boost::system::error_code &err, std::size_t bytes)
    {
        if (!err)
        {
            sock.async_read_some(boost::asio::buffer(readBuffer), std::bind(&ClientEnd::readHandler, this, std::placeholders::_1, std::placeholders::_2));
        }
        else if (err.value() == boost::system::errc::errc_t::broken_pipe)
        {
            sock.close();
        }
    }
    void connectHandler(const boost::system::error_code &err)
    {
        if (!err)
        {
            sock.async_write_some(boost::asio::buffer("Hello From Client"), std::bind(&ClientEnd::writeHandler, this, std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            char str[128];
            sprintf(str, "line %d, ErrorCode: %d; ErrorMsg: %s; Local Port: %d", __LINE__, err.value(), err.message().c_str(), sock.local_endpoint().port());
            threadSafePrint(str);
            sock.close();
        }
    }

public:
    ClientEnd(const boost::asio::ip::tcp::endpoint &ep)
        : endPoint(ep), sock(ioService)
    {
    }
    ~ClientEnd(){};
    void operator()()
    {
        sock.async_connect(endPoint, std::bind(&ClientEnd::connectHandler, this, std::placeholders::_1));
        ioService.run();
    }
};

void create_clients(const char *ipAddr, int port)
{
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ipAddr), port);
    ClientEnd client(ep);
    client();
}

int main(int argc, char *argv[])
{
    char ipServ[64] = "127.0.0.1";
    int nPort = 8008;
    int nThdCnt = 10;
    if (argc > 1)
        nThdCnt = atoi(argv[1]);
    if (argc > 2)
        sprintf(ipServ, "%s", argv[2]);
    if (argc > 3)
        nPort = atoi(argv[3]);

    std::vector<std::thread> vecThd;
    for (int i = 0; i < nThdCnt; i++)
    {
        vecThd.push_back(std::thread(create_clients, ipServ, nPort));
    }
    for (int i = 0; i < vecThd.size(); i++)
    {
        vecThd[i].join();
    }

    return 0;
}