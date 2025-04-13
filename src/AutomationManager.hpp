#pragma once
#include <map>
#include <vector>
#include <string>
#include "CurveData.hpp"

struct AutomationPoint {
    float time;
    float value;
    
    bool operator<(const AutomationPoint& other) const {
        return time < other.time;
    }
};

class AutomationManager {
public:
    static AutomationManager& getInstance() {
        static AutomationManager instance;
        return instance;
    }

    void addAutomation(const std::string& paramName, float time, float value);
    void removeAutomation(const std::string& paramName, float time);
    float getValueAtTime(const std::string& paramName, float time);
    void clearAutomation(const std::string& paramName);
    void clearAllAutomation();

private:
    AutomationManager() = default;
    std::map<std::string, std::vector<AutomationPoint>> automationData;
    float interpolateValue(const std::vector<AutomationPoint>& points, float time);
};