#include "sensorValidator.h"
#include "alarmCodes.h"
#include "iAssignmentsManager.h"
#include "iSensorManager.h"
#include <iostream>

SensorValidator::SensorValidator(IAssignmentsManager &assignments, ISensorManager &manager, IAlarmManager &alarms)
    : assignmentsManager(assignments), sensorManager(manager), alarmManager(alarms)
{
}

void SensorValidator::validateAssignedSensors()
{
    auto assignments = assignmentsManager.get(true);
    auto sensors = sensorManager.scan();

    for (const auto &[webId, sensorId] : assignments)
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
            std::cerr << "Warning: Sensor '" << sensorId << "' is assigned but not found." << std::endl;
            alarmManager.addAlarmState(sensorId, AlarmCode::SENSOR_DISCONNECTED, 0.0f);
        }
        else
        {
            std::cout << "!!!!!!!!!!!!CLEAR ALARM!!!!!!!!!!!!." << std::endl;
            alarmManager.clearAlarm(sensorId);
        }
    }

    clearAlarmsForNotExistingSensors(sensors);
}

void SensorValidator::clearAlarmsForNotExistingSensors(const std::vector<SensorData> &sensors)
{
    auto activeAlarms = alarmManager.getActiveAlarms();
    auto assignments = assignmentsManager.get();

    for (const auto &alarm : activeAlarms)
    {
        bool sensorInAssignments = false;
        bool sensorAvailable = false;

        // check if sensor is in assignments
        for (const auto &[id, sensorId] : assignments)
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

void SensorValidator::printSensorInfo()
{
    auto assignments = assignmentsManager.get();
    auto sensors = sensorManager.scan();

    std::cout << "Current sensor assignments:" << std::endl;
    for (const auto &[id, silo] : assignments)
    {
        std::cout << "  Sensor " << static_cast<int>(id) << ": " << silo << std::endl;
    }

    std::cout << "Found sensors: " << sensors.size() << std::endl;
}
