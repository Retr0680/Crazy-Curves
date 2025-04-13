#include "BenchmarkManager.hpp"
#include <algorithm>
#include <numeric>

void BenchmarkManager::startMeasurement(const std::string& name) {
    measurements[name].startTime = std::chrono::steady_clock::now();
}

void BenchmarkManager::endMeasurement(const std::string& name) {
    auto now = std::chrono::steady_clock::now();
    auto& measurement = measurements[name];
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        now - measurement.startTime).count() / 1000.0; // Convert to milliseconds
    
    measurement.samples.push_back(duration);
}

BenchmarkResult BenchmarkManager::getResults(const std::string& name) const {
    auto it = measurements.find(name);
    if (it == measurements.end() || it->second.samples.empty()) {
        return BenchmarkResult{0.0, 0.0, 0.0, 0};
    }

    const auto& samples = it->second.samples;
    double sum = std::accumulate(samples.begin(), samples.end(), 0.0);
    double avg = sum / samples.size();
    double min = *std::min_element(samples.begin(), samples.end());
    double max = *std::max_element(samples.begin(), samples.end());

    return BenchmarkResult{
        avg,
        min,
        max,
        samples.size()
    };
}

void BenchmarkManager::clearResults(const std::string& name) {
    measurements.erase(name);
}

void BenchmarkManager::clearAllResults() {
    measurements.clear();
}

void BenchmarkManager::startBenchmark(const std::string& name) {
    auto& benchmark = benchmarks[name];
    benchmark.startTime = std::chrono::high_resolution_clock::now();
    benchmark.isRunning = true;
}

void BenchmarkManager::endBenchmark(const std::string& name) {
    auto it = benchmarks.find(name);
    if (it != benchmarks.end() && it->second.isRunning) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
            (endTime - it->second.startTime);
        it->second.duration = duration.count() / 1000.0; // Convert to milliseconds
        it->second.isRunning = false;
    }
}

double BenchmarkManager::getBenchmarkResult(const std::string& name) const {
    auto it = benchmarks.find(name);
    if (it != benchmarks.end()) {
        return it->second.duration;
    }
    return 0.0;
}

void BenchmarkManager::clearBenchmarks() {
    benchmarks.clear();
}

PF_Err BenchmarkManager::benchmarkRenderOperation(
    PF_InData* in_data,
    PF_OutData* out_data,
    const std::string& operationName)
{
    PF_Err err = PF_Err_NONE;
    
    // Start timing
    startBenchmark(operationName);
    
    // Check if we need to update performance info
    if (in_data->current_time % in_data->time_step == 0) {
        // Get previous performance metrics
        auto lastDuration = getBenchmarkResult(operationName);
        
        // Log if performance degrades significantly
        if (lastDuration > 0 && getBenchmarkResult(operationName) > lastDuration * 1.5) {
            // Performance warning
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        }
    }
    
    return err;
}

PF_Err BenchmarkManager::benchmarkCurveInterpolation(
    const CurveData* curve,
    const std::string& benchmarkName)
{
    PF_Err err = PF_Err_NONE;
    
    startBenchmark(benchmarkName);
    
    // Perform test interpolations
    for (int i = 0; i < 1000; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / 999.0f;
        PF_FpLong dummy;
        
        // Evaluate curve (without storing result)
        for (A_long j = 0; j < curve->num_points - 1; j++) {
            if (x <= curve->points[j + 1].x) {
                PF_FpLong t = (x - curve->points[j].x) / 
                             (curve->points[j + 1].x - curve->points[j].x);
                dummy = curve->points[j].y + t * (curve->points[j + 1].y - curve->points[j].y);
                break;
            }
        }
    }
    
    endBenchmark(benchmarkName);
    
    return err;
}