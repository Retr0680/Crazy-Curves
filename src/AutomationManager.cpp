#include "AutomationManager.hpp"
#include <algorithm>

void AutomationManager::addAutomation(const std::string& paramName, float time, float value) {
    AutomationPoint point{time, value};
    auto& points = automationData[paramName];
    
    auto it = std::lower_bound(points.begin(), points.end(), point);
    points.insert(it, point);
}

void AutomationManager::removeAutomation(const std::string& paramName, float time) {
    auto it = automationData.find(paramName);
    if (it != automationData.end()) {
        auto& points = it->second;
        points.erase(
            std::remove_if(points.begin(), points.end(),
                [time](const AutomationPoint& p) {
                    return std::abs(p.time - time) < 0.001f;
                }),
            points.end()
        );
    }
}

float AutomationManager::getValueAtTime(const std::string& paramName, float time) {
    auto it = automationData.find(paramName);
    if (it == automationData.end() || it->second.empty()) {
        return 0.0f;
    }
    
    return interpolateValue(it->second, time);
}

void AutomationManager::clearAutomation(const std::string& paramName) {
    automationData.erase(paramName);
}

void AutomationManager::clearAllAutomation() {
    automationData.clear();
}

float AutomationManager::interpolateValue(const std::vector<AutomationPoint>& points, float time) {
    if (points.empty()) return 0.0f;
    if (points.size() == 1) return points[0].value;
    
    if (time <= points.front().time) return points.front().value;
    if (time >= points.back().time) return points.back().value;
    
    auto it = std::lower_bound(points.begin(), points.end(), AutomationPoint{time, 0.0f});
    auto next = it;
    auto prev = --it;
    
    float t = (time - prev->time) / (next->time - prev->time);
    return prev->value + (next->value - prev->value) * t;
}