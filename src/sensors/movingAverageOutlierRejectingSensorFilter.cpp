#include "movingAverageOutlierRejectingSensorFilter.h"

MAOutlierFilter::MAOutlierFilter(std::size_t windowSize, float sigmaThreshold, std::size_t warmupCount, float minTemp,
                                 float maxTemp)
    : windowSize_(windowSize > 0 ? windowSize : 1), sigmaThreshold_(sigmaThreshold > 0.0f ? sigmaThreshold : 3.0f),
      warmupCount_(warmupCount > 0 ? warmupCount : 3), minTemp_(minTemp), maxTemp_(maxTemp)
{
}

std::map<std::string, float> MAOutlierFilter::filter(const std::map<std::string, float> &rawTemps)
{
    std::map<std::string, float> result;

    for (auto const &entry : rawTemps)
    {
        auto const &sensorId = entry.first;
        float value = entry.second;

        auto &state = states_[sensorId];

        // Range check: reject values outside configured min/max (keep previous)
        if (value < minTemp_ || value > maxTemp_)
        {
            result[sensorId] = state.lastAccepted;
            continue;
        }

        // Warm-up: collect at least warmupCount_ values before rejecting outliers
        if (state.window.size() < warmupCount_)
        {
            state.window.push_back(value);
            state.sum += value;
            state.sumSquares += value * value;
            state.lastAccepted = value;
            result[sensorId] = state.sum / static_cast<float>(state.window.size());
            continue;
        }

        float mean = state.sum / static_cast<float>(state.window.size());
        float variance = (state.sumSquares / static_cast<float>(state.window.size())) - (mean * mean);
        float stdev = (variance > 0.0f) ? std::sqrt(variance) : 0.0f;

        bool isOutlier = (stdev > 0.0f) ? (std::fabs(value - mean) > sigmaThreshold_ * stdev) : false;

        if (isOutlier)
        {
            // do not add this sample to window, preserve last accepted moving average
            result[sensorId] = state.lastAccepted;
            continue;
        }

        // Accept the sample and update the window
        state.window.push_back(value);
        state.sum += value;
        state.sumSquares += value * value;

        if (state.window.size() > windowSize_)
        {
            float old = state.window.front();
            state.window.pop_front();
            state.sum -= old;
            state.sumSquares -= old * old;
        }

        float avg = state.sum / static_cast<float>(state.window.size());
        state.lastAccepted = avg;
        result[sensorId] = avg;
    }

    return result;
}
