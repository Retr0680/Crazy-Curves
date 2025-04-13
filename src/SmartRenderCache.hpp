#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "CrazyCurves.h"

class SmartRenderCache {
public:
    static PF_Err Initialize(
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        CurveData* curves);
        
    static PF_Err ProcessPixels(
        void* refcon,
        A_long xL,
        A_long yL,
        PF_Pixel8* inP,
        PF_Pixel8* outP);
        
    static PF_Err ProcessPixelsFloat(
        void* refcon,
        A_long xL,
        A_long yL,
        PF_PixelFloat* inP,
        PF_PixelFloat* outP);

private:
    static PF_Err BuildLookupTables(CurveData* curves);
    static PF_Err InterpolateValues(const CurveData* curve, PF_FpLong* values, int size);
};