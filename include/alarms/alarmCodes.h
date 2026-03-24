#pragma once

#include <cstdint>
#include <map>
#include <string>

// Alarm status codes - independent from manager to avoid dependencies
enum class AlarmCode : uint32_t
{
    NO_ALARM = 0x00,
    SENSOR_DISCONNECTED = 0x01,
    SENSOR_ERROR = 0x02,
    INVALID_DATA = 0x03,
    UNKNOWN = 0xFF
};

// Map of alarm codes to descriptions
static const std::map<AlarmCode, std::string> ALARM_CODE_DESCRIPTIONS = {
    {AlarmCode::NO_ALARM, "No alarm"},
    {AlarmCode::SENSOR_DISCONNECTED, "Sensor disconnected"},
    {AlarmCode::SENSOR_ERROR, "Sensor error"},
    {AlarmCode::INVALID_DATA, "Invalid data"},
    {AlarmCode::UNKNOWN, "Unknown alarm"}};

// Alarm severity levels
enum class AlarmSeverity : uint8_t
{
    INFO = 0,
    WARNING = 1,
    ALARM = 2,
    CRITICAL = 3
};

// Helper function to get alarm description
inline std::string getAlarmDescription(AlarmCode code)
{
    auto it = ALARM_CODE_DESCRIPTIONS.find(code);
    if (it != ALARM_CODE_DESCRIPTIONS.end())
    {
        return it->second;
    }
    return "Unknown alarm code";
}

// Helper function to get alarm severity
inline AlarmSeverity getAlarmSeverity(AlarmCode code)
{
    switch (code)
    {
    case AlarmCode::SENSOR_ERROR:
        return AlarmSeverity::ALARM;

    case AlarmCode::SENSOR_DISCONNECTED:
        return AlarmSeverity::WARNING;

    default:
        return AlarmSeverity::INFO;
    }
}
