#include "SmartRenderCache.hpp"
#include "AEGP_SuiteHandler.h"

static const int CACHE_SIZE = 256;
static PF_FpLong rgbLUT[CACHE_SIZE];
static PF_FpLong redLUT[CACHE_SIZE];
static PF_FpLong greenLUT[CACHE_SIZE];
static PF_FpLong blueLUT[CACHE_SIZE];

PF_Err SmartRenderCache::Initialize(
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    
    // Build lookup tables for each curve
    ERR(BuildLookupTables(curves));
    
    return err;
}

PF_Err SmartRenderCache::ProcessPixels(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    // Use lookup tables for fast pixel processing
    outP->alpha = inP->alpha;
    outP->red   = static_cast<A_u_char>(redLUT[inP->red]);
    outP->green = static_cast<A_u_char>(greenLUT[inP->green]);
    outP->blue  = static_cast<A_u_char>(blueLUT[inP->blue]);
    
    return PF_Err_NONE;
}

PF_Err SmartRenderCache::ProcessPixelsFloat(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    // Process floating point pixels with higher precision
    outP->alpha = inP->alpha;
    outP->red   = CurvesData::evaluate((CurveData*)refcon, inP->red);
    outP->green = CurvesData::evaluate((CurveData*)refcon, inP->green);
    outP->blue  = CurvesData::evaluate((CurveData*)refcon, inP->blue);
    
    return PF_Err_NONE;
}

PF_Err SmartRenderCache::BuildLookupTables(CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    
    // Build lookup tables for each curve
    ERR(InterpolateValues(&curves[0], rgbLUT, CACHE_SIZE));
    ERR(InterpolateValues(&curves[1], redLUT, CACHE_SIZE));
    ERR(InterpolateValues(&curves[2], greenLUT, CACHE_SIZE));
    ERR(InterpolateValues(&curves[3], blueLUT, CACHE_SIZE));
    
    return err;
}

PF_Err SmartRenderCache::InterpolateValues(
    const CurveData* curve,
    PF_FpLong* values,
    int size)
{
    for (int i = 0; i < size; ++i) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (size - 1);
        values[i] = CurvesData::evaluate(curve, x) * (size - 1);
    }
    
    return PF_Err_NONE;
}