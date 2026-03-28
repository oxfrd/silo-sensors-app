#pragma once

#include "iAlarmManager.h"
#include "iAssignmentsManager.h"
#include "iSensorManager.h"
#include "sensorData.h"
#include <vector>

class SensorValidator
{
  private:
    static constexpr auto cAbsentSensorsCleanupInterval = std::chrono::seconds(30);
    IAssignmentsManager &assignmentsManager;
    ISensorManager &sensorManager;
    IAlarmManager &alarmManager;
    std::map<uint8_t, std::string> assignments_;
    std::chrono::steady_clock::time_point lastValidation_;

    void clearAlarmsForNotExistingSensors(const std::vector<SensorData> &sensors);
    void printSensorInfo(const std::vector<SensorData> &sensors);

  public:
    SensorValidator(IAssignmentsManager &assignments, ISensorManager &manager, IAlarmManager &alarms);

    void validateAssignedSensors(bool printInfo = false);
};
