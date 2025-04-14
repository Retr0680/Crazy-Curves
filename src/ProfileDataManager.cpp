#include "ProfileDataManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

const double ProfileDataManager::PERFORMANCE_THRESHOLD_MS = 16.67; // 60fps threshold

void ProfileDataManager::BeginProfiling(const std::string& category) {
    auto& metric = metrics[category];
    metric.startTime = std::chrono::high_resolution_clock::now();
    metric.isActive = true;
}

void ProfileDataManager::EndProfiling(const std::string& category) {
    auto it = metrics.find(category);
    if (it != metrics.end() && it->second.isActive) {
        auto& metric = it->second;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - metric.startTime).count() / 1000.0; // Convert to milliseconds

        UpdateMetrics(category, duration);
    }
}

void ProfileDataManager::UpdateMetrics(const std::string& category, double duration) {
    auto& metric = metrics[category];
    metric.totalDuration += duration;
    metric.callCount++;
    metric.peakDuration = std::max(metric.peakDuration, duration);
    metric.averageDuration = metric.totalDuration / metric.callCount;
    metric.isActive = false;
}

PF_Err ProfileDataManager::AnalyzePerformance(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    bool hasPerformanceIssues = false;
    std::string report = GenerateReport();

    // Check for performance issues
    for (const auto& [category, metric] : metrics) {
        if (metric.averageDuration > PERFORMANCE_THRESHOLD_MS) {
            hasPerformanceIssues = true;
            break;
        }
    }

    // Display warning if needed
    if (hasPerformanceIssues) {
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        suites.ANSICallbacksSuite1()->sprintf(
            out_data->return_msg,
            "%s",
            report.c_str());

        // Set optimization flags
        out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_SMART_RENDER;
        out_data->out_flags2 |= PF_OutFlag2_FLOAT_COLOR_AWARE;
        out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_THREADED_RENDERING;
    }

    return err;
}

PF_Err ProfileDataManager::ExportMetrics(const std::string& filePath) {
    PF_Err err = PF_Err_NONE;

    try {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return PF_Err_INTERNAL_STRUCT_DAMAGED;
        }

        // Write CSV header
        file << "Category,Average Duration (ms),Peak Duration (ms),Call Count,Total Duration (ms)\n";

        // Write metrics
        for (const auto& [category, metric] : metrics) {
            file << category << ","
                 << std::fixed << std::setprecision(3)
                 << metric.averageDuration << ","
                 << metric.peakDuration << ","
                 << metric.callCount << ","
                 << metric.totalDuration << "\n";
        }
    }
    catch (...) {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    return err;
}

std::string ProfileDataManager::GenerateReport() const {
    std::stringstream ss;
    ss << "Performance Report:\n\n";

    for (const auto& [category, metric] : metrics) {
        ss << category << ":\n"
           << "  Average: " << std::fixed << std::setprecision(2) 
           << metric.averageDuration << "ms\n"
           << "  Peak: " << metric.peakDuration << "ms\n"
           << "  Calls: " << metric.callCount << "\n";

        if (metric.averageDuration > PERFORMANCE_THRESHOLD_MS) {
            ss << "  WARNING: Performance threshold exceeded!\n";
        }
        ss << "\n";
    }

    return ss.str();
}