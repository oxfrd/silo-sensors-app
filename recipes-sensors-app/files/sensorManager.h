#pragma once

#include "iSensorManager.h"
#include "sensorData.h"
#include "sensorInterface.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// wymagane zaleznosci do kompilacji
// g++ -std=c++17 sensors.cpp -o sensors -ljsoncpp -lstdc++fs

// w yocto recipe zaleznosci:
/*
    DEPENDS = "jsoncpp"
    EXTRA_OEMAKE = "CXXFLAGS='-std=c++17'"
*/

class SensorManager : public ISensorManager
{
  private:
    std::unique_ptr<SensorInterface> provider;

  public:
    SensorManager(std::unique_ptr<SensorInterface> customProvider = nullptr, bool mock = false);

    std::vector<SensorData> scan() override;
    std::map<std::string, float> getTemps() override;

    void setProvider(std::unique_ptr<SensorInterface> newProvider);
};
