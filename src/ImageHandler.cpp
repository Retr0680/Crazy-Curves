#include "ImageHandler.hpp"

PF_Err ImageHandler::ProcessImage8(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    ERR(suites.Iterate8Suite1()->iterate_origin(
        in_data,
        0,                          // progress base
        output->height,            // progress final
        input,                     // src 
        nullptr,                   // area - null for all pixels
        (void*)curves,             // refcon
        ProcessPixel8,             // pixel function pointer
        output));                  // dest

    return err;
}

PF_Err ImageHandler::ProcessImageFloat(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    ERR(suites.Iterate16Suite1()->iterate_origin(
        in_data,
        0,                          // progress base
        output->height,            // progress final
        input,                     // src
        nullptr,                   // area - null for all pixels
        (void*)curves,             // refcon
        ProcessPixelFloat,         // pixel function pointer
        output));                  // dest

    return err;
}

PF_Err ImageHandler::ProcessPixel8(
    void* refcon,
    A_long x,
    A_long y,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    const CurveData* curves = static_cast<const CurveData*>(refcon);
    
    // Process RGB channels
    outP->alpha = inP->alpha;
    outP->red   = static_cast<A_u_char>(CurvesData::evaluate(&curves[0], inP->red / 255.0f) * 255.0f);
    outP->green = static_cast<A_u_char>(CurvesData::evaluate(&curves[1], inP->green / 255.0f) * 255.0f);
    outP->blue  = static_cast<A_u_char>(CurvesData::evaluate(&curves[2], inP->blue / 255.0f) * 255.0f);

    return PF_Err_NONE;
}

PF_Err ImageHandler::ProcessPixelFloat(
    void* refcon,
    A_long x,
    A_long y,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    const CurveData* curves = static_cast<const CurveData*>(refcon);
    
    // Process RGB channels
    outP->alpha = inP->alpha;
    outP->red   = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[0], inP->red));
    outP->green = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[1], inP->green));
    outP->blue  = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[2], inP->blue));

    return PF_Err_NONE;
}