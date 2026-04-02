#pragma once

#include "iSensorFilter.h"

#include <chrono>
#include <map>
#include <string>

class DeltaSensorFilter : public ISensorFilter
{
  public:
    explicit DeltaSensorFilter(float deltaThreshold = 0.1f, std::chrono::seconds maxAge = std::chrono::seconds(30));

    std::map<std::string, float> filter(const std::map<std::string, float> &rawTemps) override;

  private:
    float deltaThreshold_;
    std::chrono::seconds maxAge_;
    std::map<std::string, float> lastReported_;
    std::map<std::string, std::chrono::steady_clock::time_point> lastUpdateTime_;
};
