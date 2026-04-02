#pragma once

#include "iSensorFilter.h"
#include "iSensorManager.h"
#include "sensorData.h"
#include "sensorInterface.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class SensorManager : public ISensorManager
{
  private:
    std::unique_ptr<SensorInterface> provider;
    std::unique_ptr<ISensorFilter> filter;

  public:
    SensorManager(std::unique_ptr<SensorInterface> customProvider = nullptr, bool mock = false);

    std::vector<std::string> scan() override;
    std::map<std::string, float> getTemps() override;

    void setProvider(std::unique_ptr<SensorInterface> newProvider);
    void setFilter(std::unique_ptr<ISensorFilter> filter);
};
