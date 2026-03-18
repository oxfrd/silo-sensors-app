#include <iostream>
#include <thread>
#include "assignmentsManager.h"
#include "sensorManager.h"


int main() {
    SensorManager manager(nullptr,true);
    
    auto sensors = manager.scan();
    std::cout << "Found sensors: " << sensors.size() << std::endl;
    
    for (int i = 0; i < 15; ++i) {
        auto temps = manager.getTemps();
        std::cout << "Iteration " << i << ": " << temps.size() << " measurements" << std::endl;
        for (const auto& [id, temp] : temps) {
            std::cout << "  Sensor " << id << ": " << temp << "°C" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
