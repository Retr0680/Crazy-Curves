#include "LUTManager.hpp"

PF_Err LUTManager::InitializeLUTs(const CurveData* curves) {
    PF_Err err = PF_Err_NONE;

    // Build LUTs for each curve
    ERR(BuildLUT(&curves[0], lutTables[0]));  // Master RGB
    ERR(BuildLUT(&curves[1], lutTables[1]));  // Red
    ERR(BuildLUT(&curves[2], lutTables[2]));  // Green
    ERR(BuildLUT(&curves[3], lutTables[3]));  // Blue

    if (!err) {
        lutsValid = true;
    }

    return err;
}

PF_Err LUTManager::ProcessPixel8(const PF_Pixel8* inP, PF_Pixel8* outP) const {
    if (!lutsValid) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Apply master RGB curve first
    PF_FpLong rgb_r, rgb_g, rgb_b;
    ERR(InterpolateLUT(inP->red / 255.0f, lutTables[0], &rgb_r));
    ERR(InterpolateLUT(inP->green / 255.0f, lutTables[0], &rgb_g));
    ERR(InterpolateLUT(inP->blue / 255.0f, lutTables[0], &rgb_b));

    // Then apply individual channel curves
    PF_FpLong final_r, final_g, final_b;
    ERR(InterpolateLUT(rgb_r, lutTables[1], &final_r));
    ERR(InterpolateLUT(rgb_g, lutTables[2], &final_g));
    ERR(InterpolateLUT(rgb_b, lutTables[3], &final_b));

    // Convert back to 8-bit
    outP->red = static_cast<A_u_char>(final_r * 255.0f);
    outP->green = static_cast<A_u_char>(final_g * 255.0f);
    outP->blue = static_cast<A_u_char>(final_b * 255.0f);
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

PF_Err LUTManager::ProcessPixelFloat(const PF_PixelFloat* inP, PF_PixelFloat* outP) const {
    if (!lutsValid) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Apply master RGB curve first
    PF_FpLong rgb_r, rgb_g, rgb_b;
    ERR(InterpolateLUT(inP->red, lutTables[0], &rgb_r));
    ERR(InterpolateLUT(inP->green, lutTables[0], &rgb_g));
    ERR(InterpolateLUT(inP->blue, lutTables[0], &rgb_b));

    // Then apply individual channel curves
    ERR(InterpolateLUT(rgb_r, lutTables[1], &outP->red));
    ERR(InterpolateLUT(rgb_g, lutTables[2], &outP->green));
    ERR(InterpolateLUT(rgb_b, lutTables[3], &outP->blue));
    outP->alpha = inP->alpha;

    return PF_Err_NONE;
}

void LUTManager::InvalidateLUTs() {
    lutsValid = false;
}

PF_Err LUTManager::BuildLUT(const CurveData* curve, PF_FpLong* table) {
    if (!curve || !table) return PF_Err_BAD_CALLBACK_PARAM;

    // Build lookup table
    for (A_long i = 0; i < LUT_SIZE; ++i) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (LUT_SIZE - 1);
        table[i] = CurvesData::evaluate(curve, x);
    }

    return PF_Err_NONE;
}

PF_Err LUTManager::InterpolateLUT(
    PF_FpLong input, 
    const PF_FpLong* table, 
    PF_FpLong* output) const 
{
    if (!table || !output) return PF_Err_BAD_CALLBACK_PARAM;

    // Clamp input to [0,1]
    input = input < 0.0f ? 0.0f : (input > 1.0f ? 1.0f : input);
    
    // Convert to table index
    PF_FpLong idx = input * (LUT_SIZE - 1);
    A_long idx_low = static_cast<A_long>(idx);
    A_long idx_high = idx_low + 1;
    
    if (idx_high >= LUT_SIZE) {
        *output = table[LUT_SIZE - 1];
    } else {
        // Linear interpolation between table entries
        PF_FpLong fract = idx - idx_low;
        *output = table[idx_low] + fract * (table[idx_high] - table[idx_low]);
    }

    return PF_Err_NONE;
}