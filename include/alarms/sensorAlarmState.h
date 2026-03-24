#pragma once

#include "alarmCodes.h"
#include <chrono>
#include <string>

// Structure holding alarm state for a sensor
struct SensorAlarmState
{
    AlarmCode code;
    AlarmSeverity severity;
    float sensorValue;
    std::chrono::system_clock::time_point timestamp;
    std::string sensorId;

    SensorAlarmState()
        : code(AlarmCode::NO_ALARM), severity(AlarmSeverity::INFO), sensorValue(0.0f),
          timestamp(std::chrono::system_clock::now()), sensorId("")
    {
    }

    SensorAlarmState(const std::string &id, AlarmCode c, float value)
        : code(c), severity(getAlarmSeverity(c)), sensorValue(value), timestamp(std::chrono::system_clock::now()),
          sensorId(id)
    {
    }
};
