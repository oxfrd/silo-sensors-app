#pragma once

#include "alarmManager.h"
#include "assignmentsManager.h"
#include "historyRecorder.h"
#include "sensorManager.h"

#include "iSnapshotProvider.h"
#include "udsServer.h"
#include <memory>

class MonitoringService : public ISnapshotProvider
{
  private:
    std::unique_ptr<AssignmentsManager> assignmentsManager;
    std::unique_ptr<SensorManager> sensorManager;
    std::unique_ptr<AlarmManager> alarmManager;
    std::unique_ptr<HistoryRecorder> historyRecorder;
    bool running = false;
    bool mocked_;
    int validationCounter = 0;
    mutable std::mutex dataMutex_;
    
    void dataCollector();

  public:
    MonitoringService(bool useMockedSensors = false);

    void initialize();
    void run();
    void stop();
    void start();
    Json::Value getSnapshot() const override;
};
