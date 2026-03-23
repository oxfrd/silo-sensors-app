#pragma once

#include <optional>
#include <string>

struct SensorData
{
    std::string id;
    std::optional<float> temp;

    SensorData(const std::string &id, std::optional<float> temp = std::nullopt) : id(id), temp(temp)
    {
    }
};
