#include "assignmentsManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <json/json.h> // nlohmann/json library

namespace fs = std::filesystem;

AssignmentsManager::AssignmentsManager(IAlarmManager &alarmManager, const std::string &file)
    : alarmManager_(alarmManager), storage_file(file)
{
    load();
}

// Sensors assignments manager
void AssignmentsManager::load()
{
    if (fs::exists(storage_file))
    {
        std::lock_guard<std::mutex> lock(mutex_);
        try
        {
            std::ifstream file(storage_file);
            if (file.is_open())
            {
                assignments_.clear();
                Json::Value json;
                file >> json;

                if (!json.isArray())
                {
                    std::cerr << "ERROR: Loading assignments failed: value must be arrayValue" << std::endl;
                    file.close();
                    return;
                }

                uint8_t count = 0;
                for (const auto &item : json)
                {
                    if (!item.isNull() && item.isString())
                    {
                        assignments_.emplace(count, item.asString());
                        count++;
                    }
                }
                file.close();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "ERROR: Loading assignments failed: " << e.what() << std::endl;
        }
    }
}

void AssignmentsManager::save()
{
    try
    {
        Json::Value json(Json::arrayValue);
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto &[key, value] : assignments_)
        {
            json.append(value.empty() ? Json::nullValue : Json::Value(value));
        }

        std::ofstream file(storage_file);
        if (file.is_open())
        {
            file << json;
            file.close();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: Assignment save failed: " << e.what() << std::endl;
    }
}

std::map<uint8_t, std::string> AssignmentsManager::get(bool fileReload)
{
    if (fileReload)
    {
        load();
    }

    std::lock_guard<std::mutex> lock(mutex_);
    return assignments_;
}

void AssignmentsManager::set(const std::map<uint8_t, std::string> &newAssignments)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        assignments_ = newAssignments;
    }

    save();
}

void AssignmentsManager::validateAssignedSensors(bool printInfo, const std::vector<std::string> &connectedSensors)
{
    load();

    mutex_.lock();
    auto assignmentsCopy = assignments_;
    mutex_.unlock();

    for (const auto &[webId, sensorId] : assignmentsCopy)
    {
        bool found = false;
        for (const auto &id : connectedSensors)
        {
            if (sensorId == id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Warning: Sensor '" << sensorId << "' is assigned but unconnected." << std::endl;
            alarmManager_.addAlarmState(sensorId, AlarmCode::SENSOR_DISCONNECTED, 0.0f);
        }
        else
        {
            alarmManager_.clearAlarm(sensorId);
        }
    }

    if (printInfo)
    {
        printSensorInfo();
    }

    clearAlarmsForNotExistingSensors(connectedSensors);
}

void AssignmentsManager::clearAlarmsForNotExistingSensors(const std::vector<std::string> &sensors)
{
    auto now = std::chrono::steady_clock::now();
    if (now - lastCleanup_ < cAbsentSensorsCleanupInterval)
    {
        return;
    }

    lastCleanup_ = now;

    auto activeAlarms = alarmManager_.getActiveAlarms();

    for (const auto &alarm : activeAlarms)
    {
        bool sensorInAssignments = false;
        bool sensorAvailable = false;

        // check if sensor is in assignments
        for (const auto &[id, sensorId] : assignments_)
        {
            if (alarm.sensorId == sensorId)
            {
                sensorInAssignments = true;
                break;
            }
        }

        // check if sensor is available (in the sensors list)
        for (const auto &sensor : sensors)
        {
            if (alarm.sensorId == sensor)
            {
                sensorAvailable = true;
                break;
            }
        }

        // Clear alarm only if sensor is NOT in assignments AND NOT available
        if (!sensorInAssignments && !sensorAvailable)
        {
            std::cerr << "Warning: Sensor '" << alarm.sensorId
                      << "' has alarm but not in assignments and not available. Clearing." << std::endl;
            alarmManager_.clearAlarm(alarm.sensorId);
        }
    }
}

void AssignmentsManager::printSensorInfo()
{
    std::cout << "Current sensor assignments:" << std::endl;
    for (const auto &[id, silo] : assignments_)
    {
        std::cout << "  Sensor " << static_cast<int>(id) << ": " << silo << std::endl;
    }
}
