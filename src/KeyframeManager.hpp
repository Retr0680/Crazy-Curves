#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class KeyframeManager {
public:
    static KeyframeManager& getInstance() {
        static KeyframeManager instance;
        return instance;
    }

    PF_Err AddKeyframe(
        PF_InData* in_data,
        const CurveData* curves,
        PF_FpLong time);

    PF_Err DeleteKeyframe(
        PF_FpLong time);

    PF_Err GetCurvesAtTime(
        PF_InData* in_data, 
        PF_FpLong time,
        CurveData* curves);

    PF_Err UpdateKeyframe(
        PF_InData* in_data,
        const CurveData* curves,
        PF_FpLong time);

private:
    struct Keyframe {
        PF_FpLong time;
        CurveData curves[4];  // RGB, R, G, B curves
    };

    std::vector<Keyframe> keyframes;
    
    PF_Err InterpolateCurves(
        const CurveData* c1,
        const CurveData* c2,
        PF_FpLong t,
        CurveData* result);
};