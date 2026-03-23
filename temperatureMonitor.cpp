#include "temperatureMonitor.h"
#include "iHistoryRecorder.h"
#include "iSensorManager.h"
#include <iostream>

TemperatureMonitor::TemperatureMonitor(ISensorManager &manager, IHistoryRecorder &recorder, IAlarmManager &alarms)
    : sensorManager(manager), historyRecorder(recorder), alarmManager(alarms)
{
}

void TemperatureMonitor::recordTemperatures()
{
    auto temps = sensorManager.getTemps();

    std::cout << temps.size() << " measurements" << std::endl;

    for (const auto &[id, temp] : temps)
    {
        std::cout << "  Sensor " << id << ": " << temp << "°C" << std::endl;
        historyRecorder.log(id, temp, 0);
    }
}

void TemperatureMonitor::printAlarmStatus()
{
    auto activeAlarms = alarmManager.getActiveAlarms();

    if (!activeAlarms.empty())
    {
        std::cout << "Active alarms: " << activeAlarms.size() << std::endl;
        for (const auto &alarm : activeAlarms)
        {
            std::cout << "  Sensor '" << alarm.sensorId << "' alarm code: " << static_cast<int>(alarm.code)
                      << std::endl;
        }
    }
    else
    {
        std::cout << "No active alarms." << std::endl;
    }
}
