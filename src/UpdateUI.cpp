#include "UpdateUI.hpp"

PF_Err UpdateUI::Draw(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Draw curve editor for active curve
    PF_EffectWorld* world = out_data->effect_win;
    PF_Rect bounds = {10, 10, 266, 266}; // 256x256 editor with margins

    ERR(DrawCurveEditor(in_data, world, bounds, &seqData->rgb_curve));
    ERR(DrawPoints(in_data, world, bounds, &seqData->rgb_curve));

    return err;
}

PF_Err UpdateUI::HandleMouseDown(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Convert mouse coordinates to curve space
    PF_Point mouse = event->u.do_click.screen_point;
    PF_FpLong x = (mouse.h - 10) / 256.0;
    PF_FpLong y = 1.0 - ((mouse.v - 10) / 256.0);

    // Add point to active curve
    if (x >= 0 && x <= 1 && y >= 0 && y <= 1) {
        CurvePoint newPoint = {x, y};
        CurveData* activeCurve = &seqData->rgb_curve; // Currently active curve
        
        // Add point and mark for update
        if (activeCurve->num_points < 256) {
            activeCurve->points[activeCurve->num_points++] = newPoint;
            activeCurve->dirty = true;
            out_data->refresh_flag = true;
        }
    }

    return err;
}

PF_Err UpdateUI::DrawCurveEditor(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Draw grid
    PF_Pixel gridColor = {32, 32, 32, 255};
    for (A_long i = 0; i <= 8; i++) {
        A_long pos = bounds.left + (i * 32);
        // Draw vertical line
        for (A_long y = bounds.top; y < bounds.bottom; y++) {
            suites.PF_WorldSuite1()->PF_SetPixel8(world, pos, y, &gridColor);
        }
        // Draw horizontal line
        for (A_long x = bounds.left; x < bounds.right; x++) {
            suites.PF_WorldSuite1()->PF_SetPixel8(world, x, bounds.top + (i * 32), &gridColor);
        }
    }

    return err;
}

PF_Err UpdateUI::DrawPoints(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PF_Pixel pointColor = {255, 255, 255, 255};
    
    // Draw points
    for (A_long i = 0; i < curve->num_points; i++) {
        A_long x = bounds.left + static_cast<A_long>(curve->points[i].x * 256);
        A_long y = bounds.top + static_cast<A_long>((1.0 - curve->points[i].y) * 256);
        
        // Draw 3x3 point
        for (A_long dy = -1; dy <= 1; dy++) {
            for (A_long dx = -1; dx <= 1; dx++) {
                suites.PF_WorldSuite1()->PF_SetPixel8(world, 
                    x + dx, y + dy, &pointColor);
            }
        }
    }

    return err;
}