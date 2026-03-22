#pragma once

#include "sensorInterface.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// wymagane zaleznosci do kompilacji
// g++ -std=c++17 sensors.cpp -o sensors -ljsoncpp -lstdc++fs

// w yocto recipe zaleznosci:
/*
    DEPENDS = "jsoncpp"
    EXTRA_OEMAKE = "CXXFLAGS='-std=c++17'"
*/

struct SensorData
{
    std::string id;
    std::optional<float> temp;

    SensorData(const std::string &id, std::optional<float> temp = std::nullopt) : id(id), temp(temp)
    {
    }
};

class SensorManager
{
  private:
    std::unique_ptr<SensorInterface> provider;

  public:
    SensorManager(std::unique_ptr<SensorInterface> customProvider = nullptr, bool mock = false);

    std::vector<SensorData> scan();
    std::map<std::string, float> getTemps();

    void setProvider(std::unique_ptr<SensorInterface> newProvider);
};
