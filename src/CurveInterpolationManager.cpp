#include "CurveInterpolationManager.hpp"
#include <algorithm>

PF_Err CurveInterpolationManager::InterpolateCurve(
    const CurveData* curve,
    PF_FpLong x,
    PF_FpLong* result)
{
    PF_Err err = PF_Err_NONE;

    if (!curve || !result || curve->num_points < 2) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Handle edge cases
    if (x <= curve->points[0].x) {
        *result = curve->points[0].y;
        return err;
    }
    if (x >= curve->points[curve->num_points - 1].x) {
        *result = curve->points[curve->num_points - 1].y;
        return err;
    }

    // Find segment containing x
    A_long i = 0;
    for (; i < curve->num_points - 1; i++) {
        if (x <= curve->points[i + 1].x) break;
    }

    // Use cubic interpolation when possible
    if (i > 0 && i < curve->num_points - 2) {
        PF_FpLong t = (x - curve->points[i].x) / 
                      (curve->points[i + 1].x - curve->points[i].x);
        *result = CubicInterpolate(
            curve->points[i - 1],
            curve->points[i],
            curve->points[i + 1],
            curve->points[i + 2],
            t);
    } else {
        // Fall back to linear interpolation at endpoints
        PF_FpLong t = (x - curve->points[i].x) / 
                      (curve->points[i + 1].x - curve->points[i].x);
        *result = curve->points[i].y + t * (curve->points[i + 1].y - curve->points[i].y);
    }

    return err;
}

PF_Err CurveInterpolationManager::SmoothCurve(
    CurveData* curve,
    PF_FpLong smoothness)
{
    PF_Err err = PF_Err_NONE;

    if (!curve || curve->num_points < 3) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Temporary storage for smoothed points
    CurvePoint* temp_points = new CurvePoint[curve->num_points];
    if (!temp_points) return PF_Err_OUT_OF_MEMORY;

    // Copy endpoints
    temp_points[0] = curve->points[0];
    temp_points[curve->num_points - 1] = curve->points[curve->num_points - 1];

    // Smooth interior points
    for (A_long i = 1; i < curve->num_points - 1; i++) {
        PF_FpLong cp1x, cp1y, cp2x, cp2y;
        CalculateControlPoints(curve, i, smoothness, &cp1x, &cp1y, &cp2x, &cp2y);

        temp_points[i].x = (cp1x + cp2x) / 2.0f;
        temp_points[i].y = (cp1y + cp2y) / 2.0f;
    }

    // Copy back smoothed points
    for (A_long i = 0; i < curve->num_points; i++) {
        curve->points[i] = temp_points[i];
    }

    delete[] temp_points;
    curve->dirty = true;

    return err;
}

PF_FpLong CurveInterpolationManager::CubicInterpolate(
    const CurvePoint& p0,
    const CurvePoint& p1,
    const CurvePoint& p2,
    const CurvePoint& p3,
    PF_FpLong t)
{
    PF_FpLong t2 = t * t;
    PF_FpLong t3 = t2 * t;

    // Catmull-Rom spline coefficients
    PF_FpLong a = -0.5f * p0.y + 1.5f * p1.y - 1.5f * p2.y + 0.5f * p3.y;
    PF_FpLong b = p0.y - 2.5f * p1.y + 2.0f * p2.y - 0.5f * p3.y;
    PF_FpLong c = -0.5f * p0.y + 0.5f * p2.y;
    PF_FpLong d = p1.y;

    return a * t3 + b * t2 + c * t + d;
}

void CurveInterpolationManager::CalculateControlPoints(
    const CurveData* curve,
    A_long index,
    PF_FpLong smoothness,
    PF_FpLong* cp1x,
    PF_FpLong* cp1y,
    PF_FpLong* cp2x,
    PF_FpLong* cp2y)
{
    PF_FpLong tension = 1.0f - smoothness;
    
    const CurvePoint& p0 = curve->points[index - 1];
    const CurvePoint& p1 = curve->points[index];
    const CurvePoint& p2 = curve->points[index + 1];

    *cp1x = p1.x + tension * (p0.x - p2.x) / 6.0f;
    *cp1y = p1.y + tension * (p0.y - p2.y) / 6.0f;
    *cp2x = p1.x + tension * (p2.x - p0.x) / 6.0f;
    *cp2y = p1.y + tension * (p2.y - p0.y) / 6.0f;
}