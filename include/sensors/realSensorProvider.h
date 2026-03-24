#pragma once

#include "sensorInterface.h"
#include <optional>

// Real provider DS18B20
class RealSensorProvider : public SensorInterface
{
  private:
    std::string sensorPath;
    std::vector<std::string> globSearch(const std::string &pattern);
    std::optional<float> readTemp(const std::string &sensorId);

  public:
    RealSensorProvider(const std::string &path = "/sys/bus/w1/devices");

    std::vector<std::string> scan();
    std::map<std::string, float> getTemps();
};
