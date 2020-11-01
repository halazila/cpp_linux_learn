#include <iostream>
#include <chrono>
#include <thread>
#include "SolConnServer.h"

int main()
{
    SolConnServer server;
    server.Start();
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // server.Stop();
    // server.Join();
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // std::cout << "Server Restart" << std::endl;
    // server.Start();
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // server.Stop();
    server.Join();

    return 0;
}