#include "PreviewHandler.hpp"
#include "CurveEvaluator.hpp"

PF_Err PreviewHandler::UpdatePreview(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    const PF_Rect& previewRect)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Create preview world
    PF_EffectWorld preview_world;
    ERR(suites.WorldSuite1()->new_world(
        in_data->effect_ref,
        PREVIEW_WIDTH,
        PREVIEW_HEIGHT,
        PF_WORLD_IS_DEEP(params[PARAM_INPUT]->u.ld.world),
        &preview_world));

    if (!err) {
        // Draw real-time preview
        ERR(DrawRealTimePreview(
            in_data,
            &preview_world,
            &seqData->rgb_curve,
            previewRect));

        // Copy preview to UI
        if (!err && out_data->effect_win) {
            ERR(suites.WorldSuite1()->transfer_rect(
                in_data->effect_ref,
                &preview_world,
                &preview_world.extent_hint,
                out_data->effect_win,
                &previewRect));
        }

        // Cleanup
        ERR(suites.WorldSuite1()->dispose_world(
            in_data->effect_ref,
            &preview_world));
    }

    return err;
}

PF_Err PreviewHandler::DrawRealTimePreview(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const CurveData* curves,
    const PF_Rect& bounds)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Process pixels based on color depth
    if (PF_WORLD_IS_DEEP(world)) {
        PF_PixelFloat* inP = reinterpret_cast<PF_PixelFloat*>(world->data);
        PF_PixelFloat* outP = inP;

        for (A_long y = 0; y < world->height; y++) {
            for (A_long x = 0; x < world->width; x++) {
                ERR(ProcessPreviewPixelFloat(inP++, outP++, curves));
                if (err) break;
            }
            if (err) break;
        }
    } else {
        PF_Pixel8* inP = reinterpret_cast<PF_Pixel8*>(world->data);
        PF_Pixel8* outP = inP;

        for (A_long y = 0; y < world->height; y++) {
            for (A_long x = 0; x < world->width; x++) {
                ERR(ProcessPreviewPixel8(inP++, outP++, curves));
                if (err) break;
            }
            if (err) break;
        }
    }

    return err;
}

PF_Err PreviewHandler::ProcessPreviewPixel8(
    const PF_Pixel8* inP,
    PF_Pixel8* outP,
    const CurveData* curves)
{
    return CurveEvaluator::EvaluatePixel8(curves, inP, outP);
}

PF_Err PreviewHandler::ProcessPreviewPixelFloat(
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP,
    const CurveData* curves)
{
    return CurveEvaluator::EvaluatePixelFloat(curves, inP, outP);
}