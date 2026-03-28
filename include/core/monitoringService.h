#pragma once

#include "alarmManager.h"
#include "assignmentsManager.h"
#include "historyRecorder.h"
#include "sensorManager.h"
#include "sensorValidator.h"
#include "temperatureMonitor.h"

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
    std::unique_ptr<SensorValidator> sensorValidator;
    std::unique_ptr<TemperatureMonitor> temperatureMonitor;
    std::unique_ptr<UdsServer> dataTransport;
    bool running = false;
    int validationCounter = 0;
    mutable std::mutex dataMutex_;
  public:
    MonitoringService(bool useMockedSensors = false);

    void initialize();
    void run();
    void stop();
    void start();
    Json::Value getSnapshot() const override;
};
