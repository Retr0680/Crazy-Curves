#include "RenderHandlers.hpp"

PF_Err RenderHandlers::ProcessPixel8(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red / 255.0f) * 255.0f;
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green / 255.0f) * 255.0f;
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue / 255.0f) * 255.0f;
    
    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(CurvesData::evaluate(&seqData->r_curve, rgb_r / 255.0f) * 255.0f);
    outP->green = static_cast<A_u_char>(CurvesData::evaluate(&seqData->g_curve, rgb_g / 255.0f) * 255.0f);
    outP->blue  = static_cast<A_u_char>(CurvesData::evaluate(&seqData->b_curve, rgb_b / 255.0f) * 255.0f);
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}

PF_Err RenderHandlers::ProcessPixelFloat(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red);
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green);
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue);
    
    // Then apply individual channel curves
    outP->red   = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->r_curve, rgb_r));
    outP->green = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->g_curve, rgb_g));
    outP->blue  = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->b_curve, rgb_b));
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}

PF_Err RenderHandlers::PreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult in_result;
    
    // Request input frame with required parameters
    ERR(extra->cb->checkout_layer(
        in_data->effect_ref,
        PARAM_INPUT,
        PARAM_INPUT,
        &req,
        in_data->current_time,
        in_data->time_step,
        in_data->time_scale,
        &in_result));
    
    if (!err) {
        // Set output requirements
        UnionLRect(&in_result.result_rect, &extra->output->result_rect);
        UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);
    }
    
    return err;
}

PF_Err RenderHandlers::SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // Get sequence data
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;
    
    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;
    
    // Checkout input & output buffers
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));
    
    if (!err) {
        // Process based on color depth
        if (PF_WORLD_IS_DEEP(output_worldP)) {
            ERR(suites.Iterate16Suite1()->iterate_origin(
                in_data,
                0,                          // progress base
                output_worldP->height,      // progress final
                input_worldP,               // src
                &input_worldP->extent_hint, // area to process
                seqData,                    // refcon
                ProcessPixelFloat,          // pixel function pointer
                output_worldP));            // dest
        } else {
            ERR(suites.Iterate8Suite1()->iterate_origin(
                in_data,
                0,                          // progress base
                output_worldP->height,      // progress final
                input_worldP,               // src
                &input_worldP->extent_hint, // area to process
                seqData,                    // refcon
                ProcessPixel8,              // pixel function pointer
                output_worldP));            // dest
        }
    }
    
    // Always check in input buffer
    if (input_worldP) {
        ERR(extra->cb->checkin_layer_pixels(in_data->effect_ref, PARAM_INPUT));
    }
    
    return err;
}