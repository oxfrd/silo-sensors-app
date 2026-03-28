#include "sensorValidator.h"
#include "alarmCodes.h"
#include "iAssignmentsManager.h"
#include "iSensorManager.h"
#include <iostream>

SensorValidator::SensorValidator(IAssignmentsManager &assignments, ISensorManager &manager, IAlarmManager &alarms)
    : assignmentsManager(assignments), sensorManager(manager), alarmManager(alarms)
{
    lastValidation_ = std::chrono::steady_clock::now();
}

void SensorValidator::validateAssignedSensors(bool printInfo)
{
    assignments_ = assignmentsManager.get(true);
    auto sensors = sensorManager.scan();

    for (const auto &[webId, sensorId] : assignments_)
    {
        bool found = false;
        for (const auto &id : sensors)
        {
            if (sensorId == id.id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Warning: Sensor '" << sensorId << "' is assigned but unconnected." << std::endl;
            alarmManager.addAlarmState(sensorId, AlarmCode::SENSOR_DISCONNECTED, 0.0f);
        }
        else
        {
            alarmManager.clearAlarm(sensorId);
        }
    }

    if (printInfo)
    {
        printSensorInfo(sensors);
    }

    clearAlarmsForNotExistingSensors(sensors);
}

void SensorValidator::clearAlarmsForNotExistingSensors(const std::vector<SensorData> &sensors)
{
    auto now = std::chrono::steady_clock::now();
    if (now - lastValidation_ < cAbsentSensorsCleanupInterval)
    {
        return;
    }
    
    lastValidation_ = now;

    auto activeAlarms = alarmManager.getActiveAlarms();

    for (const auto &alarm : activeAlarms)
    {
        bool sensorInAssignments = false;
        bool sensorAvailable = false;

        // check if sensor is in assignments
        for (const auto &[id, sensorId] : assignments_)
        {
            if (alarm.sensorId == sensorId)
            {
                sensorInAssignments = true;
                break;
            }
        }

        // check if sensor is available (in the sensors list)
        for (const auto &sensor : sensors)
        {
            if (alarm.sensorId == sensor.id)
            {
                sensorAvailable = true;
                break;
            }
        }

        // Clear alarm only if sensor is NOT in assignments AND NOT available
        if (!sensorInAssignments && !sensorAvailable)
        {
            std::cerr << "Warning: Sensor '" << alarm.sensorId << "' has alarm but not in assignments and not available. Clearing."
                      << std::endl;
            alarmManager.clearAlarm(alarm.sensorId);
        }
    }
}

void SensorValidator::printSensorInfo(const std::vector<SensorData> &sensors)
{
    std::cout << "Current sensor assignments:" << std::endl;
    for (const auto &[id, silo] : assignments_)
    {
        std::cout << "  Sensor " << static_cast<int>(id) << ": " << silo << std::endl;
    }

    std::cout << "Found sensors: " << sensors.size() << std::endl;
}
