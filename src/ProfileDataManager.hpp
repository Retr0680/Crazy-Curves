#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>
#include <map>
#include <chrono>

class ProfileDataManager {
public:
    static ProfileDataManager& getInstance() {
        static ProfileDataManager instance;
        return instance;
    }

    void BeginProfiling(const std::string& category);
    void EndProfiling(const std::string& category);
    
    PF_Err AnalyzePerformance(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err ExportMetrics(const std::string& filePath);

private:
    struct ProfileMetric {
        std::chrono::high_resolution_clock::time_point startTime;
        double totalDuration{0.0};
        A_long callCount{0};
        double peakDuration{0.0};
        double averageDuration{0.0};
        bool isActive{false};
    };

    std::map<std::string, ProfileMetric> metrics;
    static const double PERFORMANCE_THRESHOLD_MS;

    void UpdateMetrics(const std::string& category, double duration);
    std::string GenerateReport() const;
};