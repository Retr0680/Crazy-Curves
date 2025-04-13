#include "PerformanceMonitor.hpp"

void PerformanceMonitor::startMeasurement(const std::string& operationName) {
    auto& data = measurements[operationName];
    data.startTime = std::chrono::high_resolution_clock::now();
    data.isMeasuring = true;
}

void PerformanceMonitor::endMeasurement(const std::string& operationName) {
    auto it = measurements.find(operationName);
    if (it != measurements.end() && it->second.isMeasuring) {
        auto& data = it->second;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - data.startTime).count() / 1000.0; // Convert to milliseconds
        
        data.lastDuration = duration;
        data.totalDuration += duration;
        data.measurementCount++;
        data.isMeasuring = false;
    }
}

double PerformanceMonitor::getLastDuration(const std::string& operationName) const {
    auto it = measurements.find(operationName);
    if (it != measurements.end()) {
        return it->second.lastDuration;
    }
    return 0.0;
}

double PerformanceMonitor::getAverageDuration(const std::string& operationName) const {
    auto it = measurements.find(operationName);
    if (it != measurements.end() && it->second.measurementCount > 0) {
        return it->second.totalDuration / it->second.measurementCount;
    }
    return 0.0;
}

PF_Err PerformanceMonitor::checkPerformance(
    PF_InData* in_data,
    PF_OutData* out_data,
    const std::string& operationName)
{
    PF_Err err = PF_Err_NONE;
    
    auto lastDuration = getLastDuration(operationName);
    auto avgDuration = getAverageDuration(operationName);

    // Check for significant performance degradation
    if (lastDuration > avgDuration * 2.0 && avgDuration > 0) {
        // Set warning flag
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        
        // Update status string with performance warning
        AEGP_SuiteHandler suites(in_data->pica_basicP);
        suites.ANSICallbacksSuite1()->sprintf(
            out_data->return_msg,
            "Performance Warning: %s operation taking longer than usual (%.2fms vs avg %.2fms)",
            operationName.c_str(),
            lastDuration,
            avgDuration);
    }

    return err;
}