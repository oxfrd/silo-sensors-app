#pragma once

#include "sensorInterface.h"
#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>

class HistoryRecorder
{
  private:
    std::mutex mutex;
    std::ofstream file;
    const std::string historyFilePath;
    uint32_t saveInterval;
    uint32_t saveCounter = 0;

  public:
    HistoryRecorder(const std::string &filename, const uint32_t savingInterval = 10);

    void log(const std::string &sensorID, const float temperatureInCelsius, const uint16_t alarmCode);
};