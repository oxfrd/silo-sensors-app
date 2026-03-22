#include "historyRecorder.h"
#include <chrono>
#include <iomanip>

static constexpr uint16_t cHeaderVersion = 1;

HistoryRecorder::HistoryRecorder(const std::string &filename, const uint32_t savingInterval)
    : historyFilePath(filename), saveInterval(savingInterval)
{
    file.open(filename, std::ios::app);
    file << "timestamp,sensor_id,temperature_c,alarm_code,ver." << cHeaderVersion << std::endl;
}

void HistoryRecorder::log(const std::string &sensorID, const float temperatureInCelsius, const uint16_t alarmCode)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    file << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S") << "," << sensorID << "," << std::fixed
         << std::setprecision(2) << temperatureInCelsius << "," << alarmCode << "\n";

    saveCounter++;

    if (saveCounter % saveInterval == 0)
    {
        file.flush();
        saveCounter = 0;
    }
}
