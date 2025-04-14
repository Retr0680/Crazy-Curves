#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CurveEvaluator {
public:
    static PF_Err EvaluatePixel8(
        const CurveData* curves,
        const PF_Pixel8* inP,
        PF_Pixel8* outP);
        
    static PF_Err EvaluatePixelFloat(
        const CurveData* curves,
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP);

private:
    static PF_FpLong EvaluateCurve(
        const CurveData* curve,
        PF_FpLong x,
        bool useCache = true);
        
    static PF_FpLong InterpolateCurveSegment(
        const CurvePoint& p1,
        const CurvePoint& p2,
        PF_FpLong t);
};