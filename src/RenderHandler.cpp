#include "RenderHandler.hpp"

PF_Err RenderHandler::SmartPreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult in_result;
    
    // Checkout input buffer
    ERR(extra->cb->checkout_layer(in_data->effect_ref,
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

PF_Err RenderHandler::SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;
    
    // Checkout input & output buffers
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));
    
    if (!err) {
        AEGP_SuiteHandler suites(in_data->pica_basicP);
        SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
        
        if (PF_WORLD_IS_DEEP(output_worldP)) {
            ERR(suites.Iterate16Suite1()->iterate_origin(
                in_data,
                0,
                output_worldP->height,
                input_worldP,
                &input_worldP->extent_hint,
                seqData,
                ProcessPixelsFloat,
                output_worldP));
        } else {
            ERR(suites.Iterate8Suite1()->iterate_origin(
                in_data,
                0,
                output_worldP->height,
                input_worldP,
                &input_worldP->extent_hint,
                seqData,
                ProcessPixels8,
                output_worldP));
        }
    }
    
    // Checkin input buffer
    if (input_worldP) {
        ERR(extra->cb->checkin_layer_pixels(in_data->effect_ref, PARAM_INPUT));
    }
    
    return err;
}

PF_Err RenderHandler::ProcessPixels8(
    void* refcon,
    A_long x,
    A_long y,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply curves to each channel
    outP->alpha = inP->alpha;
    outP->red   = ApplyCurve(inP->red, &seqData->r_curve);
    outP->green = ApplyCurve(inP->green, &seqData->g_curve);
    outP->blue  = ApplyCurve(inP->blue, &seqData->b_curve);
    
    return PF_Err_NONE;
}