#pragma once

#include "iAlarmManager.h"
#include "iAssignmentsManager.h"
#include "iSensorManager.h"

class SensorValidator
{
  private:
    IAssignmentsManager &assignmentsManager;
    ISensorManager &sensorManager;
    IAlarmManager &alarmManager;

  public:
    SensorValidator(IAssignmentsManager &assignments, ISensorManager &manager, IAlarmManager &alarms);

    // Sprawdza czy wszystkie przypisane sensory są podłączone
    // Zgłasza alarm jeśli jakiś sensor nie odpowiada
    void validateAssignedSensors();

    // Wyświetla informacje o sensorach
    void printSensorInfo();
};
