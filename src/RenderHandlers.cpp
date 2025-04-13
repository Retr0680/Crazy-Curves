#include "RenderHandlers.hpp"

PF_Err ProcessPixel8(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(refcon);

    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red / 255.0f) * 255.0f;
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green / 255.0f) * 255.0f;
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue / 255.0f) * 255.0f;

    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(CurvesData::evaluate(&seqData->r_curve, rgb_r / 255.0f) * 255.0f);
    outP->green = static_cast<A_u_char>(CurvesData::evaluate(&seqData->g_curve, rgb_g / 255.0f) * 255.0f);
    outP->blue  = static_cast<A_u_char>(CurvesData::evaluate(&seqData->b_curve, rgb_b / 255.0f) * 255.0f);
    outP->alpha = inP->alpha;

    return err;
}

PF_Err ProcessPixelFloat(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(refcon);

    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red);
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green);
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue);

    // Then apply individual channel curves
    outP->red   = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->r_curve, rgb_r));
    outP->green = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->g_curve, rgb_g));
    outP->blue  = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->b_curve, rgb_b));
    outP->alpha = inP->alpha;

    return err;
}

PF_Err SmartPreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult in_result;

    // Request full-sized output buffer
    req.preserve_rgb_of_zero_alpha = true;

    ERR(extra->cb->checkout_layer(in_data->effect_ref,
                                PARAM_INPUT,
                                PARAM_INPUT,
                                &req,
                                in_data->current_time,
                                in_data->time_step,
                                in_data->time_scale,
                                &in_result));

    if (!err) {
        UnionLRect(&in_result.result_rect, &extra->output->result_rect);
        UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);
    }

    return err;
}

PF_Err SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;

    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));

    if (!err) {
        // Process based on color depth
        bool deepColor = PF_WORLD_IS_DEEP(output_worldP);

        if (deepColor) {
            ERR(suites.Iterate16Suite1()->iterate_origin(
                in_data,
                0,
                output_worldP->height,
                input_worldP,
                &input_worldP->extent_hint,
                seqData,
                ProcessPixelFloat,
                output_worldP));
        } else {
            ERR(suites.Iterate8Suite1()->iterate_origin(
                in_data,
                0,
                output_worldP->height,
                input_worldP,
                &input_worldP->extent_hint,
                seqData,
                ProcessPixel8,
                output_worldP));
        }
    }

    // Always check in input buffer
    if (input_worldP) {
        ERR(extra->cb->checkin_layer_pixels(in_data->effect_ref, PARAM_INPUT));
    }

    return err;
}