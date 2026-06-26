#include "monitoringService.h"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    bool useMockedSensors = false;

    if (argc > 1)
    {
        std::string arg = argv[1];
        if (arg == "mocked")
        {
            useMockedSensors = true;
        }
    }

    try
    {
        std::string socketPath = "/run/silo-monitor.sock";
        if (useMockedSensors)
        {
            socketPath = "/tmp/silo-monitor.sock";
        }

        MonitoringService service(useMockedSensors);
        UdsServer publisher(socketPath, service);

        service.initialize();
        publisher.start();
        service.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
