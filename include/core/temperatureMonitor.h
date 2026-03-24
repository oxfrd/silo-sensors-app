#pragma once

#include "iAlarmManager.h"
#include "iHistoryRecorder.h"
#include "iSensorManager.h"

class TemperatureMonitor
{
  private:
    ISensorManager &sensorManager;
    IHistoryRecorder &historyRecorder;
    IAlarmManager &alarmManager;

  public:
    TemperatureMonitor(ISensorManager &manager, IHistoryRecorder &recorder, IAlarmManager &alarms);

    // Pobiera aktualne temperatury i loguje je
    void recordTemperatures();

    // Wyświetla aktualne stany alarmów
    void printAlarmStatus();
};
