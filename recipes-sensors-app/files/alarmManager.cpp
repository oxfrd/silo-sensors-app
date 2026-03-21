#include "alarmManager.h"

AlarmManager::AlarmManager(){};

void AlarmManager::addAlarmState(const std::string& sensorId, AlarmCode code, float value)
{
    alarmStates[sensorId] = SensorAlarmState(sensorId, code, value);
}

SensorAlarmState AlarmManager::getAlarmState(const std::string& sensorId) const
{
    auto it = alarmStates.find(sensorId);
    if (it != alarmStates.end()) {
        return it->second;
    }
    
    // If sensor does not exist, return state without alarm
    SensorAlarmState emptyState;
    emptyState.sensorId = sensorId;
    return emptyState;
}

std::vector<SensorAlarmState> AlarmManager::getActiveAlarms() const
{
    std::vector<SensorAlarmState> activeAlarms;
    
    for (const auto& pair : alarmStates) {
        if (pair.second.code != AlarmCode::NO_ALARM) {
            activeAlarms.push_back(pair.second);
        }
    }
    
    return activeAlarms;
}

std::map<std::string, SensorAlarmState> AlarmManager::getAllAlarmStates() const
{
    return alarmStates;
}

void AlarmManager::clearAlarm(const std::string& sensorId)
{
    auto it = alarmStates.find(sensorId);
    if (it != alarmStates.end()) {
        it->second.code = AlarmCode::NO_ALARM;
        it->second.severity = AlarmSeverity::INFO;
        it->second.timestamp = std::chrono::system_clock::now();
    }
}

void AlarmManager::clearAllAlarms()
{
    for (auto& pair : alarmStates) {
        pair.second.code = AlarmCode::NO_ALARM;
        pair.second.severity = AlarmSeverity::INFO;
        pair.second.timestamp = std::chrono::system_clock::now();
    }
}

bool AlarmManager::hasActiveAlarm(const std::string& sensorId) const
{
    auto it = alarmStates.find(sensorId);
    if (it != alarmStates.end()) {
        return it->second.code != AlarmCode::NO_ALARM;
    }
    return false;
}

size_t AlarmManager::getActiveAlarmCount() const
{
    size_t count = 0;
    for (const auto& pair : alarmStates) {
        if (pair.second.code != AlarmCode::NO_ALARM) {
            count++;
        }
    }
    return count;
}

SensorAlarmState AlarmManager::getCriticalAlarm() const
{
    SensorAlarmState criticalAlarm;
    AlarmSeverity maxSeverity = AlarmSeverity::INFO;
    
    for (const auto& pair : alarmStates) {
        if (pair.second.code != AlarmCode::NO_ALARM) {
            if (pair.second.severity > maxSeverity) {
                maxSeverity = pair.second.severity;
                criticalAlarm = pair.second;
            }
        }
    }
    
    return criticalAlarm;
}

