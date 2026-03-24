#pragma once

#include "sensorData.h"
#include <map>
#include <string>
#include <vector>

// Abstract interface for managing sensors
class ISensorManager
{
  public:
    virtual ~ISensorManager() = default;

    // Scan for connected sensors
    virtual std::vector<SensorData> scan() = 0;

    // Get current temperatures for all sensors
    virtual std::map<std::string, float> getTemps() = 0;
};
