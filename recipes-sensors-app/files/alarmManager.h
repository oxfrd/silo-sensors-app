#pragma once

#include "alarmCodes.h"
#include <chrono>
#include <map>
#include <string>
#include <vector>

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

class AlarmManager
{
  public:
    AlarmManager();
    ~AlarmManager() = default;

    // Add or update alarm state for a sensor
    void addAlarmState(const std::string &sensorId, AlarmCode code, float value);

    // Get current alarm state for a specific sensor
    SensorAlarmState getAlarmState(const std::string &sensorId) const;

    // Get all active alarms (where code != NO_ALARM)
    std::vector<SensorAlarmState> getActiveAlarms() const;

    // Get all alarm states for all sensors
    std::map<std::string, SensorAlarmState> getAllAlarmStates() const;

    // Clear alarm for a specific sensor
    void clearAlarm(const std::string &sensorId);

    // Clear all alarms
    void clearAllAlarms();

    // Check if there is an active alarm for a sensor
    bool hasActiveAlarm(const std::string &sensorId) const;

    // Get count of active alarms
    size_t getActiveAlarmCount() const;

    // Get the most critical alarm (return highest severity level)
    SensorAlarmState getCriticalAlarm() const;

  private:
    // Map of alarm states: sensorId -> SensorAlarmState
    std::map<std::string, SensorAlarmState> alarmStates;
};