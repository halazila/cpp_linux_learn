#include <chrono>
#include <thread>
#include "SolcomApi.h"

int main()
{
    SolcomSpi spi;
    SolcomApi api;
    api.RegistServer("127.0.0.1", 60510);
    api.RegistSpi(&spi);
    api.Start();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    int requestID = 0;
    for (size_t i = 0; i < 10; i++)
    {
        TestCommand cmd;
        api.PostRequest(cmd, requestID++);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::this_thread::sleep_for(std::chrono::seconds(200));
    api.Stop();
    api.Join();
    return 0;
}