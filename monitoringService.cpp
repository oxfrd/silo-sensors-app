#include "monitoringService.h"
#include <chrono>
#include <iostream>
#include <map>
#include <thread>

MonitoringService::MonitoringService(bool useMockedSensors)
{
    assignmentsManager = std::make_unique<AssignmentsManager>();
    sensorManager = std::make_unique<SensorManager>(nullptr, useMockedSensors);
    alarmManager = std::make_unique<AlarmManager>();
    historyRecorder = std::make_unique<HistoryRecorder>("measurementsHistory.csv", 40);

    sensorValidator = std::make_unique<SensorValidator>(*assignmentsManager, *sensorManager, *alarmManager);
    temperatureMonitor = std::make_unique<TemperatureMonitor>(*sensorManager, *historyRecorder, *alarmManager);
}

void MonitoringService::initialize()
{
    std::cout << "Initializing monitoring service..." << std::endl;

    sensorValidator->printSensorInfo();

    auto assignments = assignmentsManager->get();
    if (assignments.empty())
    {
        assignments = {
            {0, "28ff123456789abc"}, {1, "28ffabcdef123456"}, {2, "28ffaabbccddeeff"}, {3, "28ff001122334455"}};
        assignmentsManager->set(assignments);
        std::cout << "Created default sensor assignments." << std::endl;
    }

    sensorValidator->validateAssignedSensors();

    std::cout << "Monitoring service initialized." << std::endl;

    start();
}

void MonitoringService::run()
{
    std::cout << "Starting main monitoring loop..." << std::endl;

    while (running)
    {
        temperatureMonitor->recordTemperatures();
        temperatureMonitor->printAlarmStatus();

        // Validate sensors each 5 seconds
        validationCounter++;
        if (validationCounter >= 5)
        {
            sensorValidator->validateAssignedSensors();
            validationCounter = 0;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MonitoringService::stop()
{
    std::cout << "Stopping monitoring service..." << std::endl;
    running = false;
}

void MonitoringService::start()
{
    std::cout << "Starting monitoring service..." << std::endl;
    running = true;
}
