#include "monitoringService.h"
#include "alarmManager.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

MonitoringService::MonitoringService(bool useMockedSensors) : mocked_(useMockedSensors)
{
    alarmManager = std::make_unique<AlarmManager>();
    assignmentsManager =
        std::make_unique<AssignmentsManager>(*alarmManager);
    sensorManager = std::make_unique<SensorManager>(nullptr, mocked_);
    historyRecorder = std::make_unique<HistoryRecorder>("measurementsHistory.csv", 40);
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

    auto connectedSensors = sensorManager->scan();
    assignmentsManager->validateAssignedSensors(true, connectedSensors);

    std::cout << "Monitoring service initialized." << std::endl;

    start();
}

void MonitoringService::run()
{
    using namespace std::chrono_literals;
    auto lastValidationTime = std::chrono::steady_clock::now();

    std::cout << "Starting main monitoring loop..." << std::endl;
    while (running)
    {
        
        dataCollector();
        
        if (timeElapsed(lastValidationTime, 10s))
        {
            std::cout << "Rescanning sensors and validating assignments..." << std::endl;
            auto connectedSensors = sensorManager->scan();
            assignmentsManager->validateAssignedSensors(true, connectedSensors);
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
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

bool MonitoringService::timeElapsed(std::chrono::steady_clock::time_point& last, std::chrono::milliseconds interval)
{
    auto now = std::chrono::steady_clock::now();
    if (now - last >= interval)
    {
        last = now;
        return true;
    }
    return false;
}

void MonitoringService::dataCollector()
{
    // std::lock_guard<std::mutex> lock(dataMutex_);

    auto temps = sensorManager->getTemps();

    std::cout << temps.size() << " measurements" << std::endl;
    for (const auto &[id, temp] : temps)
    {
        auto activeAlarms = alarmManager->getAlarmState(id);    
        std::cout << "  Sensor " << id << ": " << temp << "°C, Alarm: " << static_cast<int>(activeAlarms.code) << std::endl;
        historyRecorder->log(id, temp, static_cast<uint16_t>(activeAlarms.code));
    }
}