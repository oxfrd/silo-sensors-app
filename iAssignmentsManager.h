#pragma once

#include <cstdint>
#include <map>
#include <string>

// Abstract interface for managing sensor assignments
class IAssignmentsManager
{
  public:
    virtual ~IAssignmentsManager() = default;

    // Get all current assignments (ID -> Sensor address mapping)
    virtual std::map<uint8_t, std::string> get(bool fileReload = false) = 0;

    // Set new assignments
    virtual void set(const std::map<uint8_t, std::string> &newAssignments) = 0;
};
