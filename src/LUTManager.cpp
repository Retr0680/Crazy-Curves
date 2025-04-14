#include "LUTManager.hpp"
#include <algorithm>

PF_Err LUTManager::InitializeLUTs(const CurveData* curves) {
    PF_Err err = PF_Err_NONE;

    // Initialize all LUTs
    ERR(BuildLUT(&curves[0], rgbLUT));
    ERR(BuildLUT(&curves[1], redLUT));
    ERR(BuildLUT(&curves[2], greenLUT));
    ERR(BuildLUT(&curves[3], blueLUT));

    return err;
}

PF_Err LUTManager::UpdateLUTs(const CurveData* curves) {
    PF_Err err = PF_Err_NONE;

    // Update only if curves are marked as dirty
    if (curves[0].dirty) ERR(BuildLUT(&curves[0], rgbLUT));
    if (curves[1].dirty) ERR(BuildLUT(&curves[1], redLUT));
    if (curves[2].dirty) ERR(BuildLUT(&curves[2], greenLUT));
    if (curves[3].dirty) ERR(BuildLUT(&curves[3], blueLUT));

    return err;
}

PF_Err LUTManager::ProcessPixel8(const PF_Pixel8* inP, PF_Pixel8* outP) const {
    if (!rgbLUT.isValid || !redLUT.isValid || !greenLUT.isValid || !blueLUT.isValid) 
        return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Apply master RGB curve first
    PF_FpLong rgb_r = EvaluateRGB(inP->red / 255.0f);
    PF_FpLong rgb_g = EvaluateRGB(inP->green / 255.0f);
    PF_FpLong rgb_b = EvaluateRGB(inP->blue / 255.0f);

    // Then apply individual channel curves
    PF_FpLong final_r = EvaluateRed(rgb_r);
    PF_FpLong final_g = EvaluateGreen(rgb_g);
    PF_FpLong final_b = EvaluateBlue(rgb_b);

    // Convert back to 8-bit
    outP->red = static_cast<A_u_char>(final_r * 255.0f);
    outP->green = static_cast<A_u_char>(final_g * 255.0f);
    outP->blue = static_cast<A_u_char>(final_b * 255.0f);
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

PF_Err LUTManager::ProcessPixelFloat(const PF_PixelFloat* inP, PF_PixelFloat* outP) const {
    if (!rgbLUT.isValid || !redLUT.isValid || !greenLUT.isValid || !blueLUT.isValid) 
        return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Apply master RGB curve first
    PF_FpLong rgb_r = EvaluateRGB(inP->red);
    PF_FpLong rgb_g = EvaluateRGB(inP->green);
    PF_FpLong rgb_b = EvaluateRGB(inP->blue);

    // Then apply individual channel curves
    outP->red = EvaluateRed(rgb_r);
    outP->green = EvaluateGreen(rgb_g);
    outP->blue = EvaluateBlue(rgb_b);
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

void LUTManager::InvalidateLUTs() {
    rgbLUT.isValid = false;
    redLUT.isValid = false;
    greenLUT.isValid = false;
    blueLUT.isValid = false;
}

PF_FpLong LUTManager::EvaluateRGB(PF_FpLong input) const {
    return InterpolateLUT(rgbLUT, input);
}

PF_FpLong LUTManager::EvaluateRed(PF_FpLong input) const {
    return InterpolateLUT(redLUT, input);
}

PF_FpLong LUTManager::EvaluateGreen(PF_FpLong input) const {
    return InterpolateLUT(greenLUT, input);
}

PF_FpLong LUTManager::EvaluateBlue(PF_FpLong input) const {
    return InterpolateLUT(blueLUT, input);
}

PF_Err LUTManager::BuildLUT(
    const CurveData* curve,
    LUTData& lut)
{
    PF_Err err = PF_Err_NONE;

    // Generate LUT values
    for (A_long i = 0; i < LUT_SIZE; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (LUT_SIZE - 1);
        
        // Find segment containing x
        A_long segIndex = 0;
        for (; segIndex < curve->num_points - 1; segIndex++) {
            if (x <= curve->points[segIndex + 1].x) break;
        }

        // Linear interpolation within segment
        PF_FpLong x0 = curve->points[segIndex].x;
        PF_FpLong y0 = curve->points[segIndex].y;
        PF_FpLong x1 = curve->points[segIndex + 1].x;
        PF_FpLong y1 = curve->points[segIndex + 1].y;

        PF_FpLong t = (x - x0) / (x1 - x0);
        lut.values[i] = y0 + t * (y1 - y0);
    }

    lut.isValid = true;
    return err;
}

PF_FpLong LUTManager::InterpolateLUT(
    const LUTData& lut,
    PF_FpLong input) const
{
    if (!lut.isValid) return input;

    // Clamp input to [0,1]
    input = std::max(0.0, std::min(1.0, input));

    // Convert to LUT index
    PF_FpLong indexF = input * (LUT_SIZE - 1);
    A_long index0 = static_cast<A_long>(indexF);
    A_long index1 = std::min(index0 + 1, LUT_SIZE - 1);
    PF_FpLong frac = indexF - index0;

    // Linear interpolation between LUT entries
    return lut.values[index0] + frac * (lut.values[index1] - lut.values[index0]);
}