#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"

typedef struct {
    PF_FpLong x, y;
    PF_FpLong cp1x, cp1y;  // First control point
    PF_FpLong cp2x, cp2y;  // Second control point
} BezierPoint;

typedef struct {
    A_long curve_id;
    BezierPoint points[256];
    A_long num_points;
    PF_Boolean dirty;
} BezierCurveData;

class BezierCurve {
public:
    static PF_Err Initialize(BezierCurveData* curve);
    static PF_Err AddPoint(BezierCurveData* curve, PF_FpLong x, PF_FpLong y);
    static PF_Err SetControlPoints(BezierCurveData* curve, A_long index, 
                                 PF_FpLong cp1x, PF_FpLong cp1y,
                                 PF_FpLong cp2x, PF_FpLong cp2y);
    static PF_FpLong Evaluate(const BezierCurveData* curve, PF_FpLong t);
    static PF_Err Reset(BezierCurveData* curve);

private:
    static PF_FpLong BezierInterpolate(const BezierPoint& p1, const BezierPoint& p2, PF_FpLong t);
};