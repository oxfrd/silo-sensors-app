#include "monitoringService.h"
#include "alarmManager.h"
#include "sensors/movingAverageOutlierRejectingSensorFilter.h"

#include <chrono>
#include <iostream>
#include <map>
#include <thread>

MonitoringService::MonitoringService(bool useMockedSensors) : mocked_(useMockedSensors)
{
    alarmManager = std::make_unique<AlarmManager>();
    assignmentsManager = std::make_unique<AssignmentsManager>(*alarmManager);
    sensorManager = std::make_unique<SensorManager>(nullptr, mocked_);

    // Apply moving average + outlier rejection filter to stabilize readings and drop spikes.
    // min/max temperature range is validated before outlier/moving average logic.
    float minTempThld = -30.0f;
    float maxTempThld = 50.0f;
    sensorManager->setFilter(std::make_unique<MAOutlierFilter>(3, 2.0f, 3, minTempThld, maxTempThld));

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

    std::lock_guard<std::mutex> lock(dataMutex_);
    currentData_.clear();

    if (!assignments.empty())
    {
        for (const auto &sensorId : assignments)
        {
            std::cout << "Assigned sensor: " << sensorId.second << std::endl;
            currentData_.push_back(SensorData(sensorId.second));
        }
    }
    else if (!connectedSensors.empty())
    {
        std::cout << "No assignments found, publishing connected sensors instead." << std::endl;
        for (const auto &sensorId : connectedSensors)
        {
            std::cout << "Detected sensor: " << sensorId << std::endl;
            currentData_.push_back(SensorData(sensorId));
        }
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
    Json::Value root(Json::arrayValue);

    std::vector<SensorData> dataCopy;
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        dataCopy = currentData_;
    }

    for (const auto &sensorData : dataCopy)
    {
        Json::Value item;
        item["sensorId"] = sensorData.id;
        item["temperature"] = sensorData.temp ? Json::Value(*sensorData.temp) : Json::Value(Json::nullValue);
        item["alarmCode"] = sensorData.alarmCode ? Json::Value(*sensorData.alarmCode) : Json::Value(Json::nullValue);
        root.append(item);
    }

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

    auto temps = sensorManager->getFilteredTemps();

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