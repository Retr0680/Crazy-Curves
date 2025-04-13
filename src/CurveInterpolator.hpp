#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CurveInterpolator {
public:
    static PF_FpLong InterpolateCurve(
        const CurveData* curve,
        PF_FpLong x);

    static PF_Err BuildLookupTable(
        const CurveData* curve,
        PF_FpLong* table,
        A_long size);

private:
    static PF_FpLong LinearInterpolate(
        const CurvePoint& p1,
        const CurvePoint& p2,
        PF_FpLong t);
};