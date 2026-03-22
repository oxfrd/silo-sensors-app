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
