#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class InterpolationManager {
public:
    static InterpolationManager& getInstance() {
        static InterpolationManager instance;
        return instance;
    }

    PF_Err InterpolateCurves(
        const CurveData* curve1,
        const CurveData* curve2,
        PF_FpLong t,
        CurveData* result);

    PF_Err SmoothCurve(
        CurveData* curve,
        PF_FpLong tension = 0.5);

    PF_Err OptimizeCurvePoints(
        CurveData* curve,
        PF_FpLong epsilon = 0.001);

private:
    struct InterpolationCache {
        CurveData resultCurve;
        PF_FpLong lastT;
        bool isValid;
    };

    InterpolationCache cache;

    PF_Err InterpolatePoints(
        const CurvePoint& p1,
        const CurvePoint& p2,
        PF_FpLong t,
        CurvePoint* result);

    PF_Err CalculateControlPoints(
        const CurveData* curve,
        A_long index,
        PF_FpLong tension,
        PF_FpLong* cp1x, PF_FpLong* cp1y,
        PF_FpLong* cp2x, PF_FpLong* cp2y);
};