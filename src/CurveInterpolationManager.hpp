#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CurveInterpolationManager {
public:
    static PF_Err InterpolateCurve(
        const CurveData* curve,
        PF_FpLong x, 
        PF_FpLong* result);

    static PF_Err SmoothCurve(
        CurveData* curve,
        PF_FpLong smoothness);

    static PF_Err BuildLookupTable(
        const CurveData* curve,
        PF_FpLong* table,
        A_long size);

private:
    static PF_FpLong CubicInterpolate(
        const CurvePoint& p0,
        const CurvePoint& p1,
        const CurvePoint& p2,
        const CurvePoint& p3,
        PF_FpLong t);

    static void CalculateControlPoints(
        const CurveData* curve,
        A_long index,
        PF_FpLong smoothness,
        PF_FpLong* cp1x,
        PF_FpLong* cp1y,
        PF_FpLong* cp2x,
        PF_FpLong* cp2y);
};