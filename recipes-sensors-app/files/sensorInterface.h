#pragma once

#include <vector>
#include <string>
#include <map>

// Abstract interface for sensor provider
class SensorInterface {
public:
    virtual ~SensorInterface() = default;
    
    virtual std::vector<std::string> scan() = 0;
    virtual std::map<std::string, float> getTemps() = 0;
};
