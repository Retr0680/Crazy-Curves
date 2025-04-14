#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>
#include <map>
#include <chrono>

class PerformanceProfiler {
public:
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }

    void BeginProfile(const std::string& section);
    void EndProfile(const std::string& section);
    
    PF_Err GenerateReport(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err OptimizePerformance(
        PF_InData* in_data,
        PF_OutData* out_data);

private:
    struct ProfileData {
        std::chrono::high_resolution_clock::time_point startTime;
        double totalDuration{0.0};
        A_long callCount{0};
        bool isActive{false};
    };

    std::map<std::string, ProfileData> profiles;
    static const size_t REPORT_THRESHOLD_MS = 16; // 1 frame @ 60fps
};