#pragma once

#include <json/json.h>

class ISnapshotProvider
{
  public:
    virtual ~ISnapshotProvider() = default;
    virtual Json::Value getSnapshot() const = 0;
};