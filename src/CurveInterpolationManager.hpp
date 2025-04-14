#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class CurveInterpolationManager {
public:
    static CurveInterpolationManager& getInstance() {
        static CurveInterpolationManager instance;
        return instance;
    }

    enum class InterpolationType {
        Linear,
        Smooth,
        Bezier,
        Hermite,
        CatmullRom
    };

    PF_Err InterpolateCurve(
        const CurveData* curve1,
        const CurveData* curve2,
        PF_FpLong t,
        CurveData* result,
        InterpolationType type = InterpolationType::Smooth);

    PF_Err SmoothCurve(
        CurveData* curve,
        PF_FpLong tension = 0.5,
        PF_FpLong bias = 0.0,
        PF_FpLong continuity = 0.0);

    PF_Err OptimizeCurvePoints(
        CurveData* curve,
        PF_FpLong epsilon = 0.001);

private:
    struct TangentData {
        PF_FpLong inTangentX, inTangentY;
        PF_FpLong outTangentX, outTangentY;
    };

    PF_Err CalculateTangents(
        const CurveData* curve,
        std::vector<TangentData>& tangents);

    PF_Err InterpolateLinear(
        const CurvePoint& p1,
        const CurvePoint& p2,
        PF_FpLong t,
        CurvePoint* result);

    PF_Err InterpolateBezier(
        const CurvePoint& p1,
        const CurvePoint& p2,
        const TangentData& tangents,
        PF_FpLong t,
        CurvePoint* result);

    PF_Err InterpolateHermite(
        const CurvePoint& p1,
        const CurvePoint& p2,
        const TangentData& tangents,
        PF_FpLong t,
        CurvePoint* result,
        PF_FpLong tension,
        PF_FpLong bias);
};