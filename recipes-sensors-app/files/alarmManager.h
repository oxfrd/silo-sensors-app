#pragma once

#include "alarmCodes.h"
#include "iAlarmManager.h"
#include "sensorAlarmState.h"
#include <map>
#include <string>
#include <vector>

class AlarmManager : public IAlarmManager
{
  public:
    AlarmManager();
    ~AlarmManager() = default;

    // Add or update alarm state for a sensor
    void addAlarmState(const std::string &sensorId, AlarmCode code, float value) override;

    // Get current alarm state for a specific sensor
    SensorAlarmState getAlarmState(const std::string &sensorId) const;

    // Get all active alarms (where code != NO_ALARM)
    std::vector<SensorAlarmState> getActiveAlarms() const override;

    // Get all alarm states for all sensors
    std::map<std::string, SensorAlarmState> getAllAlarmStates() const;

    // Clear alarm for a specific sensor
    void clearAlarm(const std::string &sensorId) override;

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