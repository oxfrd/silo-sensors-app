#include "sensorManager.h"
#include "mockSensorProvider.h"
#include "realSensorProvider.h"
#include <filesystem>

namespace fs = std::filesystem;

SensorManager::SensorManager(std::unique_ptr<SensorInterface> customProvider, bool mock)
{
    if (mock)
    {
        provider = std::make_unique<MockSensorProvider>();
    }
    else if (customProvider)
    {
        provider = std::move(customProvider);
    }
    else
    {
        provider = std::make_unique<RealSensorProvider>();
    }
}

std::vector<std::string> SensorManager::scan()
{
    return provider->scan();
}

std::map<std::string, float> SensorManager::getTemps()
{
    auto raw = provider->getTemps();
    if (filter)
    {
        return filter->filter(raw);
    }
    return raw;
}

void SensorManager::setProvider(std::unique_ptr<SensorInterface> newProvider)
{
    provider = std::move(newProvider);
}

void SensorManager::setFilter(std::unique_ptr<ISensorFilter> newFilter)
{
    filter = std::move(newFilter);
}
