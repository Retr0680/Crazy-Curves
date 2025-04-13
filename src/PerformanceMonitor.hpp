#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include <chrono>
#include <map>
#include <string>

class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }

    void startMeasurement(const std::string& operationName);
    void endMeasurement(const std::string& operationName);
    double getLastDuration(const std::string& operationName) const;
    double getAverageDuration(const std::string& operationName) const;
    
    PF_Err checkPerformance(
        PF_InData* in_data,
        PF_OutData* out_data,
        const std::string& operationName);

private:
    struct PerformanceData {
        std::chrono::high_resolution_clock::time_point startTime;
        double lastDuration{0.0};
        double totalDuration{0.0};
        int measurementCount{0};
        bool isMeasuring{false};
    };

    std::map<std::string, PerformanceData> measurements;
};