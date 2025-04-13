#pragma once
#include <string>
#include <map>
#include <chrono>
#include <vector>

struct BenchmarkResult {
    double averageTime;
    double minTime;
    double maxTime;
    size_t sampleCount;
};

class BenchmarkManager {
public:
    static BenchmarkManager& getInstance() {
        static BenchmarkManager instance;
        return instance;
    }

    void startMeasurement(const std::string& name);
    void endMeasurement(const std::string& name);
    BenchmarkResult getResults(const std::string& name) const;
    void clearResults(const std::string& name);
    void clearAllResults();

private:
    BenchmarkManager() = default;
    struct Measurement {
        std::chrono::steady_clock::time_point startTime;
        std::vector<double> samples;
    };
    
    std::map<std::string, Measurement> measurements;
};