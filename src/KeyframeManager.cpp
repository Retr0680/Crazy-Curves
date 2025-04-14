#include "KeyframeManager.hpp"
#include <algorithm>

PF_Err KeyframeManager::AddKeyframe(
    PF_InData* in_data,
    const CurveData* curves,
    PF_FpLong time)
{
    PF_Err err = PF_Err_NONE;

    // Create new keyframe
    Keyframe keyframe;
    keyframe.time = time;

    // Copy curve data
    for (int i = 0; i < 4; i++) {
        keyframe.curves[i] = curves[i];
    }

    // Insert maintaining time order
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe);
    keyframes.insert(it, keyframe);

    return err;
}

PF_Err KeyframeManager::GetValueAtTime(
    PF_InData* in_data,
    PF_FpLong time,
    CurveData* result)
{
    PF_Err err = PF_Err_NONE;

    if (keyframes.empty()) {
        return PF_Err_INVALID_INDEX;
    }

    // Find surrounding keyframes
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), 
        Keyframe{time, {}}, 
        [](const Keyframe& a, const Keyframe& b) { 
            return a.time < b.time; 
        });

    if (it == keyframes.begin()) {
        // Before first keyframe - use first keyframe value
        *result = keyframes.front().curves[0];
    } else if (it == keyframes.end()) {
        // After last keyframe - use last keyframe value
        *result = keyframes.back().curves[0];
    } else {
        // Interpolate between surrounding keyframes
        const Keyframe& k1 = *(it - 1);
        const Keyframe& k2 = *it;

        PF_FpLong t = (time - k1.time) / (k2.time - k1.time);
        ERR(InterpolateCurves(&k1.curves[0], &k2.curves[0], t, result));
    }

    return err;
}

PF_Err KeyframeManager::DeleteKeyframe(
    PF_InData* in_data,
    PF_FpLong time)
{
    PF_Err err = PF_Err_NONE;

    auto it = std::find_if(keyframes.begin(), keyframes.end(),
        [time](const Keyframe& k) {
            return std::abs(k.time - time) < 0.001;
        });

    if (it != keyframes.end()) {
        keyframes.erase(it);
    }

    return err;
}

PF_Err KeyframeManager::InterpolateCurves(
    const CurveData* c1,
    const CurveData* c2,
    PF_FpLong t,
    CurveData* result)
{
    PF_Err err = PF_Err_NONE;

    // Initialize result curve
    result->curve_id = c1->curve_id;
    result->num_points = c1->num_points;
    result->dirty = true;

    // Interpolate each point
    for (A_long i = 0; i < c1->num_points; i++) {
        result->points[i].x = c1->points[i].x + t * (c2->points[i].x - c1->points[i].x);
        result->points[i].y = c1->points[i].y + t * (c2->points[i].y - c1->points[i].y);
    }

    return err;
}