#pragma once

#include <string>

class ISnapshotProvider
{
  public:
    virtual ~ISnapshotProvider() = default;
    virtual std::string getSnapshot() const = 0;
};