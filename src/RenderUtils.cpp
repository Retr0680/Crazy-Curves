#include "RenderUtils.hpp"

PF_Err RenderUtils::PreRenderSetup(
    PF_InData* in_data,
    PF_RenderRequest* req,
    PF_CheckoutResult* in_result)
{
    PF_Err err = PF_Err_NONE;
    
    // Set render request flags
    req->preserve_rgb_of_zero_alpha = true;
    
    // Request high-precision rendering if available
    if (in_data->quality == PF_Quality_HI) {
        req->field = PF_Field_FRAME;
    }
    
    return err;
}

PF_Err RenderUtils::RenderCurves(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves,
    const AEGP_SuiteHandler& suites)
{
    PF_Err err = PF_Err_NONE;
    
    // Determine color depth
    bool deepColor = PF_WORLD_IS_DEEP(output);
    
    if (deepColor) {
        ERR(suites.Iterate16Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            nullptr,                    // area - null for all pixels
            (void*)curves,              // refcon
            ProcessPixelFloat,          // pixel function pointer
            output));                   // dest
    } else {
        ERR(suites.Iterate8Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            nullptr,                    // area - null for all pixels
            (void*)curves,              // refcon
            ProcessPixel8,              // pixel function pointer
            output));                   // dest
    }
    
    return err;
}

PF_Err RenderUtils::ProcessPixel8(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    const CurveData* curves = static_cast<const CurveData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&curves[0], inP->red / 255.0f) * 255.0f;
    PF_FpLong rgb_g = CurvesData::evaluate(&curves[0], inP->green / 255.0f) * 255.0f;
    PF_FpLong rgb_b = CurvesData::evaluate(&curves[0], inP->blue / 255.0f) * 255.0f;
    
    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(CurvesData::evaluate(&curves[1], rgb_r / 255.0f) * 255.0f);
    outP->green = static_cast<A_u_char>(CurvesData::evaluate(&curves[2], rgb_g / 255.0f) * 255.0f);
    outP->blue  = static_cast<A_u_char>(CurvesData::evaluate(&curves[3], rgb_b / 255.0f) * 255.0f);
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}

PF_Err RenderUtils::ProcessPixelFloat(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    const CurveData* curves = static_cast<const CurveData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&curves[0], inP->red);
    PF_FpLong rgb_g = CurvesData::evaluate(&curves[0], inP->green);
    PF_FpLong rgb_b = CurvesData::evaluate(&curves[0], inP->blue);
    
    // Then apply individual channel curves
    outP->red   = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[1], rgb_r));
    outP->green = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[2], rgb_g));
    outP->blue  = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[3], rgb_b));
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}