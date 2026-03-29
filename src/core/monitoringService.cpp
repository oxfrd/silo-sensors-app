#include "monitoringService.h"
#include <chrono>
#include <iostream>
#include <map>
#include <thread>

MonitoringService::MonitoringService(bool useMockedSensors): mocked_(useMockedSensors)
{
    assignmentsManager = std::make_unique<AssignmentsManager>();
    sensorManager = std::make_unique<SensorManager>(nullptr, mocked_);
    alarmManager = std::make_unique<AlarmManager>();
    historyRecorder = std::make_unique<HistoryRecorder>("measurementsHistory.csv", 40);

    sensorValidator = std::make_unique<SensorValidator>(*assignmentsManager, *sensorManager, *alarmManager);
}

void MonitoringService::initialize()
{
    std::cout << "Initializing monitoring service..." << std::endl;

    auto assignments = assignmentsManager->get();
    if (assignments.empty() && mocked_)
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
        std::lock_guard<std::mutex> lock(dataMutex_);
        // TODO: save data here which will be transported

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

Json::Value MonitoringService::getSnapshot() const
{
    // example of implementation, TODO: expansion to real data
    std::lock_guard<std::mutex> lock(dataMutex_);
    Json::Value root;
    root["temperature"] = 23;
    root["humidity"] = 80;
    root["alarmActive"] = 0;
    return root;
}

void MonitoringService::dataCollector()
{
    //TODO: implement data collection and saving to history recorder
    auto temps = sensorManager->getTemps();
    
    std::cout << temps.size() << " measurements" << std::endl;
    for (const auto &[id, temp] : temps)
    {
        std::cout << "  Sensor " << id << ": " << temp << "°C" << std::endl;
        historyRecorder->log(id, temp, 0);
    }

    auto activeAlarms = alarmManager->getAllAlarmStates();

    if (!activeAlarms.empty())
    {
        std::cout << "Active alarms: " << activeAlarms.size() << std::endl;
        for (const auto &alarm : activeAlarms)
        {
            std::cout << "  Sensor '" << alarm.first << "' alarm code: " << static_cast<int>(alarm.second.code)
                      << std::endl;
        }
    }
    else
    {
        std::cout << "No active alarms." << std::endl;
    }
}