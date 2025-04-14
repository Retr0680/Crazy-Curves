#include "RealtimePreviewManager.hpp"
#include "CurveEvaluator.hpp"
#include "DrawingUtils.hpp"

PF_Err RealtimePreviewManager::UpdatePreview(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Create preview world
    PF_EffectWorld* preview_world = nullptr;
    ERR(CreatePreviewWorld(in_data, &preview_world));

    if (!err && preview_world) {
        // Draw grid
        PF_Rect bounds = {0, 0, PREVIEW_SIZE, PREVIEW_SIZE};
        ERR(DrawPreviewGrid(in_data, preview_world, bounds));

        // Process preview area
        if (!err) {
            ERR(ProcessPreviewArea(
                in_data,
                &params[PARAM_INPUT]->u.ld,
                preview_world,
                bounds,
                curves));
        }

        // Copy to UI
        if (!err && out_data->effect_win) {
            ERR(suites.WorldSuite1()->transfer_rect(
                in_data->effect_ref,
                preview_world,
                &bounds,
                out_data->effect_win,
                &bounds));
        }

        // Cleanup
        ERR(suites.WorldSuite1()->dispose_world(in_data->effect_ref, preview_world));
    }

    return err;
}

PF_Err RealtimePreviewManager::DrawPreviewGrid(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds)
{
    PF_Err err = PF_Err_NONE;
    
    // Draw background
    PF_Pixel8 bgColor = {32, 32, 32, 255};
    ERR(DrawingUtils::FillRect(world, bounds, bgColor));

    // Draw grid lines
    PF_Pixel8 gridColor = {64, 64, 64, 255};
    A_long stepSize = PREVIEW_SIZE / GRID_DIVISIONS;

    for (A_long i = 1; i < GRID_DIVISIONS; i++) {
        A_long pos = i * stepSize;
        
        // Vertical line
        ERR(DrawingUtils::DrawLine(
            world,
            pos, bounds.top,
            pos, bounds.bottom,
            gridColor));
            
        // Horizontal line
        ERR(DrawingUtils::DrawLine(
            world,
            bounds.left, pos,
            bounds.right, pos,
            gridColor));
    }

    return err;
}

PF_Err RealtimePreviewManager::ProcessPreviewArea(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const PF_Rect& area,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    bool deepColor = PF_WORLD_IS_DEEP(output);

    if (deepColor) {
        ERR(suites.Iterate16Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            area.bottom - area.top,     // progress final
            input,                      // src
            &area,                      // area to process
            const_cast<void*>(static_cast<const void*>(curves)), // refcon
            ProcessPreviewPixelFloat,   // pixel function pointer
            output));                   // dest
    } else {
        ERR(suites.Iterate8Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            area.bottom - area.top,     // progress final
            input,                      // src
            &area,                      // area to process
            const_cast<void*>(static_cast<const void*>(curves)), // refcon
            ProcessPreviewPixel8,       // pixel function pointer
            output));                   // dest
    }

    return err;
}

PF_Err RealtimePreviewManager::CreatePreviewWorld(
    PF_InData* in_data,
    PF_EffectWorld** world)
{
    return PF_CREATE_WORLD(
        in_data->effect_ref,
        PREVIEW_SIZE,
        PREVIEW_SIZE,
        PF_NewWorldFlag_DEEP_PIXELS,
        world);
}

PF_Err RealtimePreviewManager::ProcessPreviewPixel8(
    const PF_Pixel8* inP,
    PF_Pixel8* outP,
    const CurveData* curves)
{
    return CurveEvaluator::EvaluatePixel8(curves, inP, outP);
}

PF_Err RealtimePreviewManager::ProcessPreviewPixelFloat(
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP,
    const CurveData* curves)
{
    return CurveEvaluator::EvaluatePixelFloat(curves, inP, outP);
}