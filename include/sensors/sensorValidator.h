#pragma once

#include "iAlarmManager.h"
#include "iAssignmentsManager.h"
#include "iSensorManager.h"
#include "sensorData.h"
#include <vector>

class SensorValidator
{
  private:
    IAssignmentsManager &assignmentsManager;
    ISensorManager &sensorManager;
    IAlarmManager &alarmManager;

    void clearAlarmsForNotExistingSensors(const std::vector<SensorData> &sensors);

  public:
    SensorValidator(IAssignmentsManager &assignments, ISensorManager &manager, IAlarmManager &alarms);

    // Sprawdza czy wszystkie przypisane sensory są podłączone
    // Zgłasza alarm jeśli jakiś sensor nie odpowiada
    void validateAssignedSensors();

    // Wyświetla informacje o sensorach
    void printSensorInfo();
};
