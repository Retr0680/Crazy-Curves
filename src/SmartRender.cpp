#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

static PF_Err
SmartPreRender(
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
    
    // Set output request requirements
    if (!err) {
        UnionLRect(&in_result.result_rect, &extra->output->result_rect);
        UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);
        
        // Set up pre-render data
        PF_PreRenderData* pre_render_data = reinterpret_cast<PF_PreRenderData*>
            (AEFX_ALLOC_CLEAR(sizeof(PF_PreRenderData)));
            
        if (pre_render_data) {
            pre_render_data->input_rect = in_result.result_rect;
            extra->output->pre_render_data = pre_render_data;
        } else {
            err = PF_Err_OUT_OF_MEMORY;
        }
    }
    
    return err;
}

static PF_Err
SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    
    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    
    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;
    
    // Checkout input & output buffers
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));
    
    if (!err) {
        AEGP_SuiteHandler suites(in_data->pica_basicP);
        
        // Process based on color depth
        if (PF_WORLD_IS_DEEP(output_worldP)) {
            ERR(suites.Iterate16Suite1()->iterate_origin(
                in_data,
                0,                          // progress base
                output_worldP->height,      // progress final
                input_worldP,               // src
                &input_worldP->extent_hint, // area to process
                reinterpret_cast<void*>(seqData), // refcon
                ProcessPixelFloat,          // pixel function pointer
                output_worldP));            // destination
        } else {
            ERR(suites.Iterate8Suite1()->iterate_origin(
                in_data,
                0,                          // progress base
                output_worldP->height,      // progress final
                input_worldP,               // src
                &input_worldP->extent_hint, // area to process
                reinterpret_cast<void*>(seqData), // refcon
                ProcessPixels8,             // pixel function pointer
                output_worldP));            // destination
        }
    }
    
    // Always check out input layer pixels
    if (input_worldP) {
        ERR(extra->cb->checkin_layer_pixels(in_data->effect_ref, PARAM_INPUT));
    }
    
    return err;
}