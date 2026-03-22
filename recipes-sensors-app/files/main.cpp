#include "assignmentsManager.h"
#include "historyRecorder.h"
#include "sensorManager.h"
#include <iostream>
#include <thread>

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

    AssignmentsManager assignmentsManager = AssignmentsManager();
    SensorManager manager(nullptr, useMockedSensors);
    HistoryRecorder measurementHistory("measurementsHistory.csv", 4);

    auto assignments = assignmentsManager.get();

    std::cout << "Current sensor assignments:" << std::endl;
    for (const auto &[id, silo] : assignments)
    {
        std::cout << "  Sensor " << static_cast<int>(id) << ": " << silo << std::endl;
    }

    if (useMockedSensors && assignments.empty())
    {
        // If using mocked sensors and no assignments exist, create default ones
        assignments = {
            {0, "28ff123456789abc"}, {1, "28ffabcdef123456"}, {2, "28ffaabbccddeeff"}, {3, "28ff001122334455"}};
        assignmentsManager.set(assignments);
        std::cout << "Created default sensor assignments for mocked sensors." << std::endl;
    }

    auto sensors = manager.scan();
    std::cout << "Found sensors: " << sensors.size() << std::endl;

    for (const auto &[webId, sensorId] : assignments)
    {
        bool found = false;
        for (const auto &id : sensors)
        {
            if (sensorId == id.id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Warning: Sensor " << sensorId << " is assigned to silo but not found." << std::endl;
        }
    }

    for (;;)
    {
        auto temps = manager.getTemps();
        std::cout << temps.size() << " measurements" << std::endl;

        for (const auto &[id, temp] : temps)
        {
            std::cout << "  Sensor " << id << ": " << temp << "°C" << std::endl;

            measurementHistory.log(id, temp, 0);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
