#pragma once

#include <cstdint>
#include <limits>
#include <map>
#include <string>

class AssignmentsManager
{
  private:
    static constexpr std::uint8_t MAX_SENSORS_COUNT = std::numeric_limits<std::uint8_t>::max();

    std::string storage_file;
    std::uint8_t sensor_count = MAX_SENSORS_COUNT;
    std::map<uint8_t, std::string> assignments;

    void load();

  public:
    AssignmentsManager(const std::string &file = "silo_assignments.json");

    void save();
    std::map<uint8_t, std::string> get();
    void set(const std::map<uint8_t, std::string> &newAssignments);
};
