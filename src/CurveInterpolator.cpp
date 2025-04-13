#include "CurveInterpolator.hpp"

PF_FpLong CurveInterpolator::InterpolateCurve(
    const CurveData* curve,
    PF_FpLong x)
{
    // Handle edge cases
    if (!curve || curve->num_points < 2) return x;
    if (x <= curve->points[0].x) return curve->points[0].y;
    if (x >= curve->points[curve->num_points - 1].x) {
        return curve->points[curve->num_points - 1].y;
    }

    // Find segment containing x
    A_long i = 0;
    for (; i < curve->num_points - 1; i++) {
        if (x <= curve->points[i + 1].x) break;
    }

    // Interpolate between points
    PF_FpLong t = (x - curve->points[i].x) / 
                  (curve->points[i + 1].x - curve->points[i].x);
    return LinearInterpolate(curve->points[i], 
                           curve->points[i + 1], t);
}

PF_Err CurveInterpolator::BuildLookupTable(
    const CurveData* curve,
    PF_FpLong* table,
    A_long size)
{
    PF_Err err = PF_Err_NONE;

    if (!curve || !table || size <= 0) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Build lookup table
    for (A_long i = 0; i < size; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (size - 1);
        table[i] = InterpolateCurve(curve, x);
    }

    return err;
}

PF_FpLong CurveInterpolator::LinearInterpolate(
    const CurvePoint& p1,
    const CurvePoint& p2,
    PF_FpLong t)
{
    // Clamp t to [0,1]
    t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
    return p1.y + t * (p2.y - p1.y);
}