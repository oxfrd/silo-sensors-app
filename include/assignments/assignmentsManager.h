#pragma once

#include "iAssignmentsManager.h"
#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <mutex>
#include <chrono>
#include <vector>

class AssignmentsManager : public IAssignmentsManager
{
  private:
    static constexpr auto cAbsentSensorsCleanupInterval = std::chrono::seconds(30);

    std::string storage_file;
    std::map<uint8_t, std::string> assignments_;
    std::chrono::steady_clock::time_point lastValidation_;
    std::chrono::steady_clock::time_point lastCleanup_;
    std::chrono::seconds validationInterval_;
    std::mutex mutex_;

    void load();
    void clearAlarmsForNotExistingSensors(const std::vector<std::string> &sensors);
    void printSensorInfo();

  public:
    AssignmentsManager(const std::string &file = "silo_assignments.json", std::chrono::seconds validationInterval = std::chrono::seconds(10));

    void save();
    std::map<uint8_t, std::string> get(bool fileReload = false) override;
    void set(const std::map<uint8_t, std::string> &newAssignments) override;
    void validateAssignedSensors(bool printInfo, const std::vector<std::string> &connectedSensors);
};
