#include <iostream>
#include <future>
#include "zmq.hpp"

class zmqsocketmonitor : public zmq::monitor_t
{
private:
    /* data */
public:
    zmqsocketmonitor(/* args */) {}
    ~zmqsocketmonitor() {}
    void on_event_connected(const zmq_event_t &event_, const char *addr_) override
    {
        std::cout << "on connected" << std::endl;
    }
    void on_event_disconnected(const zmq_event_t &event_, const char *addr_) override
    {
        std::cout << "on disconnected" << std::endl;
    }
};

int main()
{
    zmq::context_t context(1);
    zmq::socket_t client(context, ZMQ_DEALER);
    client.connect("tcp://127.0.0.1:5559");
    zmqsocketmonitor climonitor;
    //async run monitor
    // auto _ = std::async(std::launch::async, [&]() {
    //     climonitor.monitor(client, "inproc://monitor-client", ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    // });

    //init and then check_event
    climonitor.init(client, "inproc://monitor-client", ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    while (1)
    {
        climonitor.check_event(1000);
        std::cout << "********" << std::endl;
    }

    return 0;
}