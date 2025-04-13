#include "AnimationManager.hpp"
#include <algorithm>

void AnimationManager::addKeyframe(float time, const CurveData& curve) {
    Keyframe keyframe{time, curve};
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe);
    keyframes.insert(it, keyframe);
}

void AnimationManager::removeKeyframe(float time) {
    auto it = std::find_if(keyframes.begin(), keyframes.end(),
        [time](const Keyframe& k) { return std::abs(k.time - time) < 0.001f; });
    
    if (it != keyframes.end()) {
        keyframes.erase(it);
    }
}

CurveData AnimationManager::interpolateAtTime(float time) {
    if (keyframes.empty()) {
        return CurveData();
    }
    
    if (keyframes.size() == 1 || time <= keyframes.front().time) {
        return keyframes.front().curve;
    }
    
    if (time >= keyframes.back().time) {
        return keyframes.back().curve;
    }
    
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), Keyframe{time, CurveData()});
    auto next = it;
    auto prev = --it;
    
    float t = (time - prev->time) / (next->time - prev->time);
    
    // Interpolate between curves
    CurveData result;
    const auto& points1 = prev->curve.getPoints();
    const auto& points2 = next->curve.getPoints();
    
    for (size_t i = 0; i < points1.size(); ++i) {
        float x = points1[i].x + (points2[i].x - points1[i].x) * t;
        float y = points1[i].y + (points2[i].y - points1[i].y) * t;
        result.addPoint(x, y);
    }
    
    return result;
}

void AnimationManager::clear() {
    keyframes.clear();
}

float AnimationManager::getDuration() const {
    return keyframes.empty() ? 0.0f : keyframes.back().time;
}