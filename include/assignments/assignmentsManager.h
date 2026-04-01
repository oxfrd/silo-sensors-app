#pragma once

#include "iAlarmManager.h"
#include "iAssignmentsManager.h"

#include <chrono>
#include <cstdint>
#include <limits>
#include <map>
#include <mutex>
#include <string>
#include <vector>

class AssignmentsManager : public IAssignmentsManager
{
  private:
    static constexpr auto cAbsentSensorsCleanupInterval = std::chrono::seconds(30);

    IAlarmManager &alarmManager_;
    std::string storage_file;
    std::map<uint8_t, std::string> assignments_;
    std::chrono::steady_clock::time_point lastCleanup_;
    std::mutex mutex_;

    void load();
    void clearAlarmsForNotExistingSensors(const std::vector<std::string> &sensors);
    void printSensorInfo();

  public:
    AssignmentsManager(IAlarmManager &alarmManager, const std::string &file = "silo_assignments.json");

    void save();
    std::map<uint8_t, std::string> get(bool fileReload = false) override;
    void set(const std::map<uint8_t, std::string> &newAssignments) override;
    void validateAssignedSensors(bool printInfo, const std::vector<std::string> &connectedSensors);
};
