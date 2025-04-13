#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include <chrono>
#include <string>
#include <map>

class BenchmarkManager {
public:
    static BenchmarkManager& getInstance() {
        static BenchmarkManager instance;
        return instance;
    }

    void startBenchmark(const std::string& name);
    void endBenchmark(const std::string& name);
    double getBenchmarkResult(const std::string& name) const;
    void clearBenchmarks();
    
    // Specific AE benchmarks
    PF_Err benchmarkRenderOperation(
        PF_InData* in_data,
        PF_OutData* out_data,
        const std::string& operationName);

    PF_Err benchmarkCurveInterpolation(
        const CurveData* curve,
        const std::string& benchmarkName);

private:
    BenchmarkManager() = default;
    struct BenchmarkData {
        std::chrono::high_resolution_clock::time_point startTime;
        double duration{0.0};
        bool isRunning{false};
    };
    
    std::map<std::string, BenchmarkData> benchmarks;
};