#include "RenderCache.hpp"

RenderCache::RenderCache() : isValid(false) {
    Invalidate();
}

RenderCache::~RenderCache() {}

PF_Err RenderCache::Initialize(const CurveData* curves) {
    PF_Err err = PF_Err_NONE;

    // Build lookup tables for all curves
    ERR(BuildLookupTable(&curves[0], rgbLUT));  // RGB curve
    ERR(BuildLookupTable(&curves[1], redLUT));  // Red curve
    ERR(BuildLookupTable(&curves[2], greenLUT));  // Green curve
    ERR(BuildLookupTable(&curves[3], blueLUT));  // Blue curve

    if (!err) {
        isValid = true;
    }

    return err;
}

PF_Err RenderCache::ProcessPixel8(PF_Pixel8* inP, PF_Pixel8* outP) const {
    if (!isValid) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Apply RGB curve first
    PF_FpLong rgb_r = rgbLUT[inP->red];
    PF_FpLong rgb_g = rgbLUT[inP->green];
    PF_FpLong rgb_b = rgbLUT[inP->blue];

    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(redLUT[static_cast<A_long>(rgb_r * 255.0f)]);
    outP->green = static_cast<A_u_char>(greenLUT[static_cast<A_long>(rgb_g * 255.0f)]);
    outP->blue  = static_cast<A_u_char>(blueLUT[static_cast<A_long>(rgb_b * 255.0f)]);
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

PF_Err RenderCache::ProcessPixelFloat(PF_PixelFloat* inP, PF_PixelFloat* outP) const {
    if (!isValid) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Convert float values to lookup table indices
    A_long r_idx = static_cast<A_long>(inP->red * (CACHE_SIZE - 1));
    A_long g_idx = static_cast<A_long>(inP->green * (CACHE_SIZE - 1));
    A_long b_idx = static_cast<A_long>(inP->blue * (CACHE_SIZE - 1));

    // Clamp indices
    r_idx = MIN(MAX(r_idx, 0), CACHE_SIZE - 1);
    g_idx = MIN(MAX(g_idx, 0), CACHE_SIZE - 1);
    b_idx = MIN(MAX(b_idx, 0), CACHE_SIZE - 1);

    // Apply curves using lookup tables
    outP->red   = rgbLUT[r_idx];
    outP->green = rgbLUT[g_idx];
    outP->blue  = rgbLUT[b_idx];
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

void RenderCache::Invalidate() {
    // Initialize lookup tables with identity mapping
    for (A_long i = 0; i < CACHE_SIZE; ++i) {
        PF_FpLong value = static_cast<PF_FpLong>(i) / (CACHE_SIZE - 1);
        rgbLUT[i] = redLUT[i] = greenLUT[i] = blueLUT[i] = value;
    }
    isValid = false;
}

PF_Err RenderCache::BuildLookupTable(const CurveData* curve, PF_FpLong* table) {
    if (!curve || !table) return PF_Err_BAD_CALLBACK_PARAM;

    for (A_long i = 0; i < CACHE_SIZE; ++i) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (CACHE_SIZE - 1);
        table[i] = CurvesData::evaluate(curve, x);
    }

    return PF_Err_NONE;
}