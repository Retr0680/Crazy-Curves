#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "CrazyCurves.h"
#include <vector>

class AnimationManager {
public:
    struct Keyframe {
        PF_FpLong time;
        CurveData curve;
    };

    struct KeyframeCache {
        PF_Boolean dirty;
        PF_FpLong cachedTime;
        CurveData cachedCurve;
    };

    AnimationManager();

    PF_Err AddKeyframe(
        PF_InData* in_data,
        const CurveData* curve,
        PF_FpLong time);

    PF_Err RemoveKeyframe(PF_FpLong time);

    PF_Err EvaluateAtTime(
        PF_InData* in_data,
        PF_FpLong time,
        CurveData* result);

private:
    std::vector<Keyframe> keyframes;
    KeyframeCache keyframeCache;
    PF_FpLong currentTime;

    PF_Err InterpolateCurves(
        const CurveData* c1,
        const CurveData* c2,
        PF_FpLong t,
        CurveData* result);

    void InitializeKeyframeCache();
};