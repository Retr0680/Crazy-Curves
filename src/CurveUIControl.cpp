#include "CurveUIControl.hpp"

PF_Err CurveUIControl::Draw(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    
    ERR(DrawGrid(in_data, world, bounds));
    ERR(DrawCurve(in_data, world, bounds, curve));
    ERR(DrawPoints(in_data, world, bounds, curve));
    
    return err;
}

PF_Err CurveUIControl::DrawGrid(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    PF_Pixel8 gridColor = {32, 32, 32, 255};
    
    for (A_long i = 0; i <= GRID_DIVISIONS; ++i) {
        A_long x = bounds.left + (i * (bounds.right - bounds.left) / GRID_DIVISIONS);
        A_long y = bounds.top + (i * (bounds.bottom - bounds.top) / GRID_DIVISIONS);
        
        // Draw vertical line
        for (A_long py = bounds.top; py < bounds.bottom; ++py) {
            ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, x, py, &gridColor));
        }
        
        // Draw horizontal line
        for (A_long px = bounds.left; px < bounds.right; ++px) {
            ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, px, y, &gridColor));
        }
    }
    
    return err;
}

PF_Err CurveUIControl::DrawCurve(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    PF_Pixel8 curveColor = {255, 255, 255, 255};
    A_long width = bounds.right - bounds.left;
    
    PF_FpLong prevY = CurvesData::evaluate(curve, 0.0);
    
    for (A_long x = 1; x < width; ++x) {
        PF_FpLong t = static_cast<PF_FpLong>(x) / width;
        PF_FpLong y = CurvesData::evaluate(curve, t);
        
        A_long px1 = bounds.left + x - 1;
        A_long px2 = bounds.left + x;
        A_long py1 = bounds.bottom - static_cast<A_long>(prevY * (bounds.bottom - bounds.top));
        A_long py2 = bounds.bottom - static_cast<A_long>(y * (bounds.bottom - bounds.top));
        
        // Draw line segment
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, px1, py1, &curveColor));
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, px2, py2, &curveColor));
        
        prevY = y;
    }
    
    return err;
}

PF_Err CurveUIControl::DrawPoints(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    PF_Pixel8 pointColor = {255, 128, 0, 255};
    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;
    
    for (A_long i = 0; i < curve->num_points; ++i) {
        A_long px = bounds.left + static_cast<A_long>(curve->points[i].x * width);
        A_long py = bounds.bottom - static_cast<A_long>(curve->points[i].y * height);
        
        // Draw point marker
        for (A_long dy = -POINT_SIZE; dy <= POINT_SIZE; ++dy) {
            for (A_long dx = -POINT_SIZE; dx <= POINT_SIZE; ++dx) {
                if (dx*dx + dy*dy <= POINT_SIZE*POINT_SIZE) {
                    ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, 
                        px + dx, py + dy, &pointColor));
                }
            }
        }
    }
    
    return err;
}