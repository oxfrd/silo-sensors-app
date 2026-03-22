#pragma once

#include "alarmCodes.h"
#include "sensorAlarmState.h"
#include <map>
#include <string>
#include <vector>

// Abstract interface for managing alarms
class IAlarmManager
{
  public:
    virtual ~IAlarmManager() = default;

    // Add or update alarm state for a sensor
    virtual void addAlarmState(const std::string &sensorId, AlarmCode code, float value) = 0;

    // Get all active alarms (where code != NO_ALARM)
    virtual std::vector<SensorAlarmState> getActiveAlarms() const = 0;

    virtual void clearAlarm(const std::string &sensorId) = 0;
};
