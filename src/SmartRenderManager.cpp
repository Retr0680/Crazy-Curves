#include "SmartRenderManager.hpp"
#include "CurveEvaluator.hpp"
#include "CacheManager.hpp"

PF_Err SmartRenderManager::PreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra,
    PF_RenderRequest* req)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // Get sequence data
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Create smart render data
    SmartRenderData* smartData = new SmartRenderData();
    if (!smartData) return PF_Err_OUT_OF_MEMORY;

    // Copy curve data
    smartData->curves[0] = seqData->rgb_curve;
    smartData->curves[1] = seqData->r_curve;
    smartData->curves[2] = seqData->g_curve;
    smartData->curves[3] = seqData->b_curve;

    // Set up render request
    req->preserve_rgb_of_zero_alpha = true;
    
    // High quality rendering if in high quality mode
    if (in_data->quality == PF_Quality_HI) {
        req->field = PF_Field_FRAME;
    }

    // Store smart render data
    extra->output->pre_render_data = smartData;

    return err;
}

PF_Err SmartRenderManager::SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Retrieve smart render data
    SmartRenderData* smartData = static_cast<SmartRenderData*>(extra->input->pre_render_data);
    if (!smartData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Get input & output worlds
    PF_EffectWorld* input = NULL;
    PF_EffectWorld* output = NULL;
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output));

    if (!err && input && output) {
        // Process the render area
        ERR(ProcessSmartRenderChunk(
            in_data,
            smartData,
            input,
            output,
            &input->extent_hint));
    }

    // Always check in input buffer
    if (input) {
        ERR(extra->cb->checkin_layer_pixels(in_data->effect_ref, PARAM_INPUT));
    }

    // Cleanup
    delete smartData;

    return err;
}

PF_Err SmartRenderManager::ProcessSmartRenderChunk(
    PF_InData* in_data,
    const SmartRenderData* renderData,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const PF_Rect* area)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Process based on color depth
    bool deepColor = PF_WORLD_IS_DEEP(output);
    
    if (deepColor) {
        ERR(suites.Iterate16Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            area,                       // area to process
            const_cast<void*>(static_cast<const void*>(renderData)), // refcon
            CurveEvaluator::EvaluatePixelFloat,  // pixel function pointer
            output));                   // dest
    } else {
        ERR(suites.Iterate8Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            area,                       // area to process
            const_cast<void*>(static_cast<const void*>(renderData)), // refcon
            CurveEvaluator::EvaluatePixel8,      // pixel function pointer
            output));                   // dest
    }

    return err;
}