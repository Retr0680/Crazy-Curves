#include "CurveRenderHandlers.hpp"

PF_Err CurveRenderHandlers::RenderCurve(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve,
    const PF_Pixel8& color)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;

    // Draw curve segments
    for (A_long x = 0; x < width; x++) {
        PF_FpLong t = static_cast<PF_FpLong>(x) / width;
        PF_FpLong y = CurvesData::evaluate(curve, t);
        
        A_long screenY = bounds.bottom - static_cast<A_long>(y * height);
        A_long screenX = bounds.left + x;

        if (screenX >= 0 && screenX < world->width &&
            screenY >= 0 && screenY < world->height) {
            ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, screenX, screenY, &color));
        }
    }

    return err;
}

PF_Err CurveRenderHandlers::RenderPoints(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve,
    const PF_Pixel8& color,
    A_long selectedPoint)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;

    // Draw control points
    for (A_long i = 0; i < curve->num_points; i++) {
        A_long pointSize = (i == selectedPoint) ? SELECTED_POINT_SIZE : POINT_SIZE;
        
        A_long screenX = bounds.left + static_cast<A_long>(curve->points[i].x * width);
        A_long screenY = bounds.bottom - static_cast<A_long>(curve->points[i].y * height);

        // Draw point marker
        for (A_long dy = -pointSize; dy <= pointSize; dy++) {
            for (A_long dx = -pointSize; dx <= pointSize; dx++) {
                if (dx*dx + dy*dy <= pointSize*pointSize) {
                    A_long px = screenX + dx;
                    A_long py = screenY + dy;
                    
                    if (px >= 0 && px < world->width &&
                        py >= 0 && py < world->height) {
                        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, px, py, &color));
                    }
                }
            }
        }
    }

    return err;
}