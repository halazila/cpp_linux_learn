#include <thread>
#include "DispatcherService.h"

int main(int argc, char *argv[])
{
    DispatcherService service;
    service.SetPort(9859);
    service.StartService();

    return 0;
}