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