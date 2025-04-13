#pragma once

#include "AE_Effect.h"
#include "CrazyCurves.h"

class CurvesData {
public:
    static PF_Err initCurve(CurveData *curve);
    static PF_Err addPoint(CurveData *curve, PF_FpLong x, PF_FpLong y);
    static PF_Err removePoint(CurveData *curve, PF_FpLong x, PF_FpLong y);
    static PF_FpLong evaluate(const CurveData *curve, PF_FpLong x);
    static PF_Err reset(CurveData *curve);

private:
    static PF_FpLong interpolate(PF_FpLong x, const CurvePoint& p1, const CurvePoint& p2);
    static void sortPoints(CurveData *curve);
};