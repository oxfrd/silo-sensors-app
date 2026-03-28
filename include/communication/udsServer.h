#pragma once

#include "iSnapshotProvider.h"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

class UdsServer
{
  public:
    UdsServer(std::string socketPath, ISnapshotProvider &provider,
              std::chrono::milliseconds minInterval = std::chrono::milliseconds(250));
    ~UdsServer();

    void start();
    void stop();

  private:
    void loop();
    bool createAndBindSocket();
    void cleanup();

    std::string socketPath_;
    ISnapshotProvider &provider_;
    std::thread worker_;
    std::atomic<bool> running_{false};
    int serverFd_{-1};
    std::chrono::steady_clock::time_point lastSendTime_{};
    std::chrono::milliseconds minInterval_;
};