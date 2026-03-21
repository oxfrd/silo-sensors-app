#include <iostream>
#include <thread>
#include "assignmentsManager.h"
#include "sensorManager.h"
#include "historyRecorder.h"

int main(int argc, char* argv[]) {
    bool useMockedSensors = false;
    
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "mocked") {
            useMockedSensors = true;
        }
    }

    SensorManager manager(nullptr, useMockedSensors);
    HistoryRecorder measurementHistory("measurementsHistory.csv", 4);
    auto sensors = manager.scan();
    std::cout << "Found sensors: " << sensors.size() << std::endl;
    
    for (;;) 
    {
        auto temps = manager.getTemps();
        std::cout << temps.size() << " measurements" << std::endl;
        
        for (const auto& [id, temp] : temps) 
        {
            std::cout << "  Sensor " << id << ": " << temp << "°C" << std::endl;
            
            measurementHistory.log(id, temp, 0);
            
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
