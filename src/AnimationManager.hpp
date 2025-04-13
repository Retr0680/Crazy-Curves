#pragma once
#include <vector>
#include <chrono>
#include "CurveData.hpp"

struct Keyframe {
    float time;
    CurveData curve;
    
    bool operator<(const Keyframe& other) const {
        return time < other.time;
    }
};

class AnimationManager {
public:
    static AnimationManager& getInstance() {
        static AnimationManager instance;
        return instance;
    }

    void addKeyframe(float time, const CurveData& curve);
    void removeKeyframe(float time);
    CurveData interpolateAtTime(float time);
    void clear();
    float getDuration() const;
    
private:
    AnimationManager() = default;
    std::vector<Keyframe> keyframes;
    std::chrono::steady_clock::time_point startTime;
};