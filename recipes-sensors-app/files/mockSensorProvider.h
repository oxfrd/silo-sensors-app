#pragma once

#include "sensorInterface.h"
#include <random>

class MockSensorProvider : public SensorInterface
{
  private:
    double global_time;
    std::mt19937 gen{std::random_device{}()};

  public:
    MockSensorProvider();

    std::vector<std::string> scan() override;

    std::map<std::string, float> getTemps() override;
};
