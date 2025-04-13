#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

static PF_Err 
ProcessPixel8(
    void* refcon,
    A_long x,
    A_long y,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(refcon);

    // Apply RGB curve first
    PF_FpLong rgb_r = seqData->rgb_curve.points[inP->red].y;
    PF_FpLong rgb_g = seqData->rgb_curve.points[inP->green].y;
    PF_FpLong rgb_b = seqData->rgb_curve.points[inP->blue].y;

    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(seqData->r_curve.points[static_cast<A_long>(rgb_r * 255)].y * 255);
    outP->green = static_cast<A_u_char>(seqData->g_curve.points[static_cast<A_long>(rgb_g * 255)].y * 255);
    outP->blue  = static_cast<A_u_char>(seqData->b_curve.points[static_cast<A_long>(rgb_b * 255)].y * 255);
    outP->alpha = inP->alpha;

    return err;
}

static PF_Err
ProcessPixelFloat(
    void* refcon,
    A_long x,
    A_long y,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(refcon);

    // Apply RGB curve first using interpolation
    PF_FpLong rgb_r = interpolateCurve(&seqData->rgb_curve, inP->red);
    PF_FpLong rgb_g = interpolateCurve(&seqData->rgb_curve, inP->green);
    PF_FpLong rgb_b = interpolateCurve(&seqData->rgb_curve, inP->blue);

    // Then apply individual channel curves
    outP->red   = interpolateCurve(&seqData->r_curve, rgb_r);
    outP->green = interpolateCurve(&seqData->g_curve, rgb_g);
    outP->blue  = interpolateCurve(&seqData->b_curve, rgb_b);
    outP->alpha = inP->alpha;

    return err;
}

PF_Err 
Render(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PF_EffectWorld* input_worldP = &params[PARAM_INPUT]->u.ld;
    PF_EffectWorld* output_worldP = output;

    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    bool deepColor = PF_WORLD_IS_DEEP(output);

    if (deepColor) {
        ERR(suites.Iterate16Suite1()->iterate(
            in_data,
            0,                          // progress base
            output_worldP->height,      // progress final
            input_worldP,               // src 
            nullptr,                    // area - null for all pixels
            (void*)seqData,            // refcon
            ProcessPixelFloat,          // pixel function pointer
            output_worldP));            // dest
    } else {
        ERR(suites.Iterate8Suite1()->iterate(
            in_data,
            0,                          // progress base
            output_worldP->height,      // progress final
            input_worldP,               // src
            nullptr,                    // area - null for all pixels
            (void*)seqData,            // refcon
            ProcessPixel8,              // pixel function pointer
            output_worldP));            // dest
    }

    return err;
}