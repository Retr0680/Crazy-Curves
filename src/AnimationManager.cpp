#include "AnimationManager.hpp"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include <algorithm>

AnimationManager::AnimationManager() : currentTime(0) {
    InitializeKeyframeCache();
}

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

PF_Err AnimationManager::AddKeyframe(
    PF_InData* in_data,
    const CurveData* curve,
    PF_FpLong time) 
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Create new keyframe
    Keyframe keyframe;
    keyframe.time = time;
    keyframe.curve = *curve;

    // Insert keyframe in sorted order
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });

    keyframes.insert(it, keyframe);
    keyframeCache.dirty = true;

    return err;
}

PF_Err AnimationManager::RemoveKeyframe(PF_FpLong time) 
{
    PF_Err err = PF_Err_NONE;

    auto it = std::find_if(keyframes.begin(), keyframes.end(),
        [time](const Keyframe& k) { return std::abs(k.time - time) < 0.001f; });

    if (it != keyframes.end()) {
        keyframes.erase(it);
        keyframeCache.dirty = true;
    }

    return err;
}

PF_Err AnimationManager::EvaluateAtTime(
    PF_InData* in_data,
    PF_FpLong time,
    CurveData* result) 
{
    PF_Err err = PF_Err_NONE;

    // Find surrounding keyframes
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), time,
        [](const Keyframe& k, PF_FpLong t) { return k.time < t; });

    if (it == keyframes.begin()) {
        *result = keyframes.front().curve;
    } else if (it == keyframes.end()) {
        *result = keyframes.back().curve;
    } else {
        const Keyframe& k1 = *(it - 1);
        const Keyframe& k2 = *it;

        // Interpolate between keyframes
        PF_FpLong t = (time - k1.time) / (k2.time - k1.time);
        ERR(InterpolateCurves(&k1.curve, &k2.curve, t, result));
    }

    return err;
}

PF_Err AnimationManager::InterpolateCurves(
    const CurveData* c1,
    const CurveData* c2,
    PF_FpLong t,
    CurveData* result)
{
    PF_Err err = PF_Err_NONE;

    // Copy curve structure
    result->curve_id = c1->curve_id;
    result->num_points = c1->num_points;
    result->dirty = true;

    // Interpolate points
    for (A_long i = 0; i < c1->num_points; i++) {
        result->points[i].x = c1->points[i].x + t * (c2->points[i].x - c1->points[i].x);
        result->points[i].y = c1->points[i].y + t * (c2->points[i].y - c1->points[i].y);
    }

    return err;
}

void AnimationManager::InitializeKeyframeCache() {
    keyframeCache.dirty = true;
    keyframeCache.cachedTime = -1.0;
}