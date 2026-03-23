#pragma once

#include <cstdint>
#include <string>

// Abstract interface for recording temperature history
class IHistoryRecorder
{
  public:
    virtual ~IHistoryRecorder() = default;

    // Log a temperature measurement
    virtual void log(const std::string &sensorID, const float temperatureInCelsius, const uint16_t alarmCode) = 0;
};
