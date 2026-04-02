#pragma once

#include "assignmentsManager.h"
#include "historyRecorder.h"
#include "sensorManager.h"

#include "iAlarmManager.h"
#include "iSnapshotProvider.h"
#include "udsServer.h"
#include <chrono>
#include <memory>

class MonitoringService : public ISnapshotProvider
{
  private:
    std::unique_ptr<IAlarmManager> alarmManager;
    std::unique_ptr<AssignmentsManager> assignmentsManager;
    std::unique_ptr<SensorManager> sensorManager;
    std::unique_ptr<HistoryRecorder> historyRecorder;
    bool running = false;
    bool mocked_;
    mutable std::mutex dataMutex_;

    std::vector<SensorData> currentData_;

    void dataCollector();

    bool timeElapsed(std::chrono::steady_clock::time_point &last, std::chrono::milliseconds interval);

  public:
    MonitoringService(bool useMockedSensors = false);

    void initialize();
    void run();
    void stop();
    void start();
    Json::Value getSnapshot() const override;
};
