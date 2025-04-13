#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class RenderCache {
public:
    static const A_long CACHE_SIZE = 256;

    RenderCache();
    ~RenderCache();

    PF_Err Initialize(const CurveData* curves);
    PF_Err ProcessPixel8(PF_Pixel8* inP, PF_Pixel8* outP) const;
    PF_Err ProcessPixelFloat(PF_PixelFloat* inP, PF_PixelFloat* outP) const;
    void Invalidate();

private:
    PF_FpLong rgbLUT[CACHE_SIZE];
    PF_FpLong redLUT[CACHE_SIZE];
    PF_FpLong greenLUT[CACHE_SIZE];
    PF_FpLong blueLUT[CACHE_SIZE];
    bool isValid;

    PF_Err BuildLookupTable(const CurveData* curve, PF_FpLong* table);
};