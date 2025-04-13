#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include "SmartRender.hpp"

PF_Err PreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult in_result;

    // Request input frame with full color info
    req.preserve_rgb_of_zero_alpha = true;

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
        UnionLRect(&in_result.result_rect, &extra->output->result_rect);
        UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);
        
        // Store render data
        CurveData* preRenderData = new CurveData();
        if (preRenderData) {
            SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
            *preRenderData = seqData->rgb_curve;  // Copy current curve data
            extra->output->pre_render_data = preRenderData;
        } else {
            err = PF_Err_OUT_OF_MEMORY;
        }
    }

    return err;
}

PF_Err SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // Get sequence data
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;

    // Checkout input & output buffers
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));

    if (!err && input_worldP && output_worldP) {
        // Process based on color depth
        bool deepColor = PF_WORLD_IS_DEEP(output_worldP);

        if (deepColor) {
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