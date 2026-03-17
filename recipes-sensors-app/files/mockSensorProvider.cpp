#include "mockSensorProvider.h"
#include <chrono>
#include <iostream>

// Mock provider for testing without real sensors
class MockSensorProvider : public SensorInterface {
private:
    double global_time;
    std::mt19937 gen{std::random_device{}()};
    
public:
    MockSensorProvider() : global_time(std::chrono::system_clock::now().time_since_epoch().count()) {}
    
    std::vector<std::string> scan() override {
        return {
            "28ff123456789abc",
            "28ffabcdef123456",
            "28ffaabbccddeeff",
            "28ff001122334455"
        };
    }
    
    std::map<std::string, float> getTemps() override {
        global_time += 2.0;  // Simulate 2s delays
        
        std::uniform_real_distribution<float> noise01(-0.2f, 0.2f);
        std::uniform_real_distribution<float> noise02(-0.1f, 0.1f);
        
        std::map<std::string, float> temps = {
            {"28ff123456789abc", std::round((22.5f + noise01(gen)) * 10.0f) / 10.0f},
            {"28ffabcdef123456", std::round((21.1f + noise01(gen)) * 10.0f) / 10.0f},
            {"28ffaabbccddeeff", std::round((19.8f + noise01(gen)) * 10.0f) / 10.0f},
            {"28ff001122334455", std::round((17.3f + noise02(gen)) * 10.0f) / 10.0f}
        };
        
        std::cout << "🌐 Global time: " << global_time << ", temps: " << temps.size() << " sensors" << std::endl;
        return temps;
    }
};
