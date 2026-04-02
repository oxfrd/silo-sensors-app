#pragma once

#include <optional>
#include <string>
#include <cstdint>

struct SensorData
{
    std::string id;
    std::optional<float> temp;
    std::optional<std::uint16_t> alarmCode;

    SensorData(const std::string &id, std::optional<float> temp = std::nullopt, std::optional<std::uint16_t> alarmCode = std::nullopt) : id(id), temp(temp), alarmCode(alarmCode) {}
};
