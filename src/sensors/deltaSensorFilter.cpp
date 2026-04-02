#include "deltaSensorFilter.h"

#include <cmath>

DeltaSensorFilter::DeltaSensorFilter(float deltaThreshold, std::chrono::seconds maxAge)
    : deltaThreshold_(deltaThreshold), maxAge_(maxAge)
{
}

std::map<std::string, float> DeltaSensorFilter::filter(const std::map<std::string, float> &rawTemps)
{
    auto now = std::chrono::steady_clock::now();
    std::map<std::string, float> result;

    for (const auto &entry : rawTemps)
    {
        const auto &sensorId = entry.first;
        float rawValue = entry.second;

        auto prevIt = lastReported_.find(sensorId);
        auto timeIt = lastUpdateTime_.find(sensorId);

        if (prevIt == lastReported_.end())
        {
            result[sensorId] = rawValue;
            lastReported_[sensorId] = rawValue;
            lastUpdateTime_[sensorId] = now;
            continue;
        }

        float lastValue = prevIt->second;
        auto lastTime = timeIt->second;
        float diff = std::abs(rawValue - lastValue);

        if (diff >= deltaThreshold_ || (now - lastTime) >= maxAge_)
        {
            result[sensorId] = rawValue;
            lastReported_[sensorId] = rawValue;
            lastUpdateTime_[sensorId] = now;
        }
        else
        {
            result[sensorId] = lastValue;
        }
    }

    return result;
}
