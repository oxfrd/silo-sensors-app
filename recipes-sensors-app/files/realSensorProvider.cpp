#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <glob.h>
#include <filesystem>
#include <optional>
#include <cmath>
#include <random>
#include <chrono>
#include <json/json.h>  // nlohmann/json library

namespace fs = std::filesystem;

// Real provider DS18B20
class RealSensorProvider : public SensorInterface {
private:
    std::vector<std::string> globSearch(const std::string& pattern) {
        std::vector<std::string> results;
        glob_t globResult = {};
        
        if (glob(pattern.c_str(), GLOB_TILDE, nullptr, &globResult) == 0) {
            for (size_t i = 0; i < globResult.glPathc; ++i) {
                results.push_back(globResult.glPathv[i]);
            }
        }
        globfree(&globResult);
        return results;
    }
    
    std::optional<float> readTemp(const std::string& sensorId) {
        try {
            std::string w1SlavePath = sensorPath + "/" + sensorId + "/w1_slave";
            std::ifstream file(w1SlavePath);
            
            if (!file.is_open()) {
                std::cerr << "❌ File not found: " << w1SlavePath << std::endl;
                return std::nullopt;
            }
            
            std::string line1, line2;
            std::getline(file, line1);
            std::getline(file, line2);
            file.close();
            
            if (line1.find("YES") == std::string::npos) {
                std::cout << "⚠️ " << sensorId << ": CRC failed" << std::endl;
                return std::nullopt;
            }
            
            size_t pos = line2.find("t=");
            if (pos != std::string::npos) {
                int tempRaw = std::stoi(line2.substr(pos + 2));
                float temp = std::round((tempRaw / 1000.0f) * 100.0f) / 100.0f;
                std::cout << "✅ " << sensorId << ": " << temp << "°C" << std::endl;
                return temp;
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Read fail " << sensorId << ": " << e.what() << std::endl;
        }
        return std::nullopt;
    }
    
public:
    RealSensorProvider(const std::string& path = "/sys/bus/w1/devices")
        : sensorPath(path) {}
    
    std::vector<std::string> scan() override {
        std::vector<std::string> sensors;
        try {
            auto paths = globSearch(sensorPath + "/28-*");
            for (const auto& path : paths) {
                size_t pos = path.rfind('/');
                if (pos != std::string::npos) {
                    sensors.push_back(path.substr(pos + 1));
                }
            }
            std::cout << "🔍 Found sensors: " << sensors.size() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "❌ Scanning failed: " << e.what() << std::endl;
        }
        return sensors;
    }
    
    std::map<std::string, float> getTemps() override {
        std::map<std::string, float> temps;
        for (const auto& sensorId : scan()) {
            auto temp = readTemp(sensorId);
            if (temp.has_value()) {
                temps[sensorId] = temp.value();
            } else {
                std::cout << "⚠️ " << sensorId << ": Brak odczytu" << std::endl;
            }
        }
        return temps;
    }
};
