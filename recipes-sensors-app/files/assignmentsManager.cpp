#include "assignmentsManager.h"
#include <filesystem>
#include <fstream>
#include <json/json.h>  // nlohmann/json library
#include <iostream>

namespace fs = std::filesystem;

// Sensors assignments manager
void AssignmentsManager::load() {
    if (fs::exists(storage_file)) {
        try {
            std::ifstream file(storage_file);
            if (file.is_open()) {
                Json::Value json;
                file >> json;
                
                uint8_t count = 0;
                for (auto& key : json.getMemberNames()) {
                    if (!json[key].isNull()) {
                        assignments.emplace(count, json[key].asString());
                        count++;
                    }
                }
                file.close();
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Loading assignments failed: " << e.what() << std::endl;
        }
    }
}

    
AssignmentsManager::AssignmentsManager(const std::string& file)
    : storage_file(file) {}

void AssignmentsManager::save() {
    try {
        Json::Value json;
        for (const auto& [key, value] : assignments) {
            json[key] = value.empty() ? Json::nullValue : Json::Value(value);
        }
        
        std::ofstream file(storage_file);
        if (file.is_open()) {
            file << json;
            file.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Assignment save failed: " << e.what() << std::endl;
    }
}

std::map<uint8_t, std::string> AssignmentsManager::get() {
    return assignments;
}

void AssignmentsManager::set(const std::map<uint8_t, std::string>& newAssignments) {
    assignments = newAssignments;
    save();
}
