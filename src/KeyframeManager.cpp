#include "KeyframeManager.hpp"
#include "InterpolationManager.hpp"

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
    auto it = std::lower_bound(keyframes.begin(), keyframes.end(), keyframe,
        [](const Keyframe& a, const Keyframe& b) {
            return a.time < b.time;
        });

    keyframes.insert(it, keyframe);

    return err;
}

PF_Err KeyframeManager::GetCurvesAtTime(
    PF_InData* in_data,
    PF_FpLong time,
    CurveData* curves)
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
        // Before first keyframe - use first keyframe
        for (int i = 0; i < 4; i++) {
            curves[i] = keyframes.front().curves[i];
        }
    }
    else if (it == keyframes.end()) {
        // After last keyframe - use last keyframe
        for (int i = 0; i < 4; i++) {
            curves[i] = keyframes.back().curves[i];
        }
    }
    else {
        // Interpolate between keyframes
        const Keyframe& k1 = *(it - 1);
        const Keyframe& k2 = *it;
        PF_FpLong t = (time - k1.time) / (k2.time - k1.time);

        for (int i = 0; i < 4; i++) {
            ERR(InterpolateCurves(&k1.curves[i], &k2.curves[i], t, &curves[i]));
        }
    }

    return err;
}

PF_Err KeyframeManager::DeleteKeyframe(PF_FpLong time)
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

PF_Err KeyframeManager::UpdateKeyframe(
    PF_InData* in_data,
    const CurveData* curves,
    PF_FpLong time)
{
    PF_Err err = PF_Err_NONE;

    auto it = std::find_if(keyframes.begin(), keyframes.end(),
        [time](const Keyframe& k) {
            return std::abs(k.time - time) < 0.001;
        });

    if (it != keyframes.end()) {
        // Update existing keyframe
        for (int i = 0; i < 4; i++) {
            it->curves[i] = curves[i];
        }
    }
    else {
        // Add new keyframe if not found
        ERR(AddKeyframe(in_data, curves, time));
    }

    return err;
}

PF_Err KeyframeManager::InterpolateCurves(
    const CurveData* c1,
    const CurveData* c2,
    PF_FpLong t,
    CurveData* result)
{
    return InterpolationManager::getInstance().InterpolateCurves(c1, c2, t, result);
}