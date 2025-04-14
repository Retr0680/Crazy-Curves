#include "PerformanceProfiler.hpp"

void PerformanceProfiler::BeginProfile(const std::string& section) {
    auto& profile = profiles[section];
    profile.startTime = std::chrono::high_resolution_clock::now();
    profile.isActive = true;
}

void PerformanceProfiler::EndProfile(const std::string& section) {
    auto it = profiles.find(section);
    if (it != profiles.end() && it->second.isActive) {
        auto& profile = it->second;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - profile.startTime).count() / 1000.0; // Convert to milliseconds

        profile.totalDuration += duration;
        profile.callCount++;
        profile.isActive = false;
    }
}

PF_Err PerformanceProfiler::GenerateReport(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    std::string report = "Performance Report:\n";
    bool hasPerformanceIssues = false;

    for (const auto& [section, data] : profiles) {
        double avgDuration = data.totalDuration / data.callCount;
        report += section + ": " +
                 std::to_string(avgDuration) + "ms (avg) x " +
                 std::to_string(data.callCount) + " calls\n";

        if (avgDuration > REPORT_THRESHOLD_MS) {
            hasPerformanceIssues = true;
            report += "WARNING: Performance threshold exceeded!\n";
        }
    }

    // Display report if there are performance issues
    if (hasPerformanceIssues) {
        out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        suites.ANSICallbacksSuite1()->sprintf(
            out_data->return_msg,
            "%s",
            report.c_str());
    }

    return err;
}

PF_Err PerformanceProfiler::OptimizePerformance(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;

    // Analyze performance data and apply optimizations
    for (const auto& [section, data] : profiles) {
        double avgDuration = data.totalDuration / data.callCount;

        if (avgDuration > REPORT_THRESHOLD_MS) {
            // Set optimization flags based on performance data
            if (section == "Render") {
                out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_SMART_RENDER;
                out_data->out_flags |= PF_OutFlag_PIX_INDEPENDENT;
            }
            else if (section == "CurveEvaluation") {
                // Enable lookup table optimization
                out_data->out_flags2 |= PF_OutFlag2_FLOAT_COLOR_AWARE;
            }
            else if (section == "PixelProcessing") {
                // Enable parallel processing
                out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_THREADED_RENDERING;
            }
        }
    }

    return err;
}