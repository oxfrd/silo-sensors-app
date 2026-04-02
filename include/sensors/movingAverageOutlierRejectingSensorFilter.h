#pragma once

#include "iSensorFilter.h"

#include <cmath>
#include <deque>
#include <map>
#include <string>

class MAOutlierFilter : public ISensorFilter
{
  public:
    MAOutlierFilter(std::size_t windowSize = 5, float sigmaThreshold = 3.0f, std::size_t warmupCount = 3,
                    float minTemp = -273.15f, float maxTemp = 1000.0f);

    std::map<std::string, float> filter(const std::map<std::string, float> &rawTemps) override;

  private:
    struct SensorState
    {
        std::deque<float> window;
        float sum = 0.0f;
        float sumSquares = 0.0f;
        float lastAccepted = 0.0f;
    };

    std::size_t windowSize_;
    float sigmaThreshold_;
    std::size_t warmupCount_;
    float minTemp_;
    float maxTemp_;
    std::map<std::string, SensorState> states_;
};
