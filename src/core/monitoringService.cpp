#include "monitoringService.h"
#include "alarmManager.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

MonitoringService::MonitoringService(bool useMockedSensors) : mocked_(useMockedSensors)
{
    alarmManager = std::make_unique<AlarmManager>();
    assignmentsManager = std::make_unique<AssignmentsManager>(*alarmManager);
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

    for (const auto &sensorId : assignments)
    {
        std::cout << "Assigned sensor: " << sensorId.second << std::endl;
        std::lock_guard<std::mutex> lock(dataMutex_);
        currentData_.push_back(SensorData(sensorId.second));
    }

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

bool MonitoringService::timeElapsed(std::chrono::steady_clock::time_point &last, std::chrono::milliseconds interval)
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
    std::vector<SensorData> dataCopy;
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        dataCopy = currentData_;
    }

    auto temps = sensorManager->getTemps();

    for (auto &sensorData : dataCopy)
    {
        auto activeAlarms = alarmManager->getAlarmState(sensorData.id);
        sensorData.temp = temps.at(sensorData.id);
        sensorData.alarmCode = static_cast<std::uint16_t>(activeAlarms.code);

        std::cout << "  Sensor " << sensorData.id << ": " << sensorData.temp.value_or(0.0f)
                  << "°C, Alarm: " << static_cast<std::uint16_t>(activeAlarms.code) << std::endl;

        historyRecorder->log(sensorData.id, sensorData.temp.value_or(0.0f),
                             static_cast<std::uint16_t>(activeAlarms.code));
    }

    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        currentData_ = std::move(dataCopy);
    }
}