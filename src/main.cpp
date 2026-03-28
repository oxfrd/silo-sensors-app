#include "monitoringService.h"
#include <iostream>

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
        MonitoringService service(useMockedSensors);
        UdsServer publisher("/run/silo-monitor.sock", service);

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
