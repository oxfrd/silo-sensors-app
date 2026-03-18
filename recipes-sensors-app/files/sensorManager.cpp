#include <filesystem>
#include "sensorManager.h"
#include "realSensorProvider.h"
#include "mockSensorProvider.h"

namespace fs = std::filesystem;

// wymagane zaleznosci do kompilacji
// g++ -std=c++17 sensors.cpp -o sensors -ljsoncpp -lstdc++fs

// w yocto recipe zaleznosci:
/*
    DEPENDS = "jsoncpp"
    EXTRA_OEMAKE = "CXXFLAGS='-std=c++17'"
*/

    
SensorManager::SensorManager(std::unique_ptr<SensorInterface> customProvider, bool mock) {
    if (mock) {
        provider = std::make_unique<MockSensorProvider>();
    } else if (customProvider) {
        provider = std::move(customProvider);
    } else {
        provider = std::make_unique<RealSensorProvider>();
    }
}

std::vector<SensorData> SensorManager::scan() {
    std::vector<SensorData> result;
    auto ids = provider->scan();
    for (const auto& id : ids) {
        result.emplace_back(id);
    }
    return result;
}

std::map<std::string, float> SensorManager::getTemps() {
    return provider->getTemps();
}

void SensorManager::setProvider(std::unique_ptr<SensorInterface> newProvider) {
    provider = std::move(newProvider);
}
