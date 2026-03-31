#pragma once

#include <map>
#include <string>
#include <vector>

// Abstract interface for managing sensors
class ISensorManager
{
  public:
    virtual ~ISensorManager() = default;

    // Scan for connected sensors
    virtual std::vector<std::string> scan() = 0;

    // Get current temperatures for all sensors
    virtual std::map<std::string, float> getTemps() = 0;
};
