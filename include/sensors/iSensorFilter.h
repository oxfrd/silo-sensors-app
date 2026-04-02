#pragma once

#include <map>
#include <string>

class ISensorFilter
{
  public:
    virtual ~ISensorFilter() = default;

    // Apply filtering to raw temperature readings.
    // Always returns a value for every key in rawTemps.
    virtual std::map<std::string, float> filter(const std::map<std::string, float> &rawTemps) = 0;
};
