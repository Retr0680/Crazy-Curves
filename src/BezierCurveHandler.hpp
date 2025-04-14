#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class BezierCurveHandler {
public:
    static PF_Err EvaluateBezier(
        const CurvePoint& p0,
        const CurvePoint& p1, 
        const CurvePoint& p2,
        const CurvePoint& p3,
        PF_FpLong t,
        PF_FpLong* result);

    static PF_Err CalculateControlPoints(
        const CurveData* curve,
        A_long index,
        PF_FpLong tension,
        PF_FpLong* cp1x, PF_FpLong* cp1y,
        PF_FpLong* cp2x, PF_FpLong* cp2y);

    static PF_Err SmoothCurve(
        CurveData* curve,
        PF_FpLong smoothness);

private:
    static PF_FpLong BernsteinPolynomial(int i, PF_FpLong t);
};