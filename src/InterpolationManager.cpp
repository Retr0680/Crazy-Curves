#include "InterpolationManager.hpp"
#include <algorithm>
#include <cmath>

PF_Err InterpolationManager::InterpolateCurves(
    const CurveData* curve1,
    const CurveData* curve2,
    PF_FpLong t,
    CurveData* result)
{
    PF_Err err = PF_Err_NONE;

    // Check cache validity
    if (cache.isValid && std::abs(t - cache.lastT) < 0.0001) {
        *result = cache.resultCurve;
        return err;
    }

    // Initialize result curve
    result->num_points = std::max(curve1->num_points, curve2->num_points);
    result->dirty = true;

    // Interpolate each point
    for (A_long i = 0; i < result->num_points; i++) {
        const CurvePoint& p1 = (i < curve1->num_points) ? 
            curve1->points[i] : curve1->points[curve1->num_points - 1];
        const CurvePoint& p2 = (i < curve2->num_points) ? 
            curve2->points[i] : curve2->points[curve2->num_points - 1];

        ERR(InterpolatePoints(p1, p2, t, &result->points[i]));
    }

    // Update cache
    if (!err) {
        cache.resultCurve = *result;
        cache.lastT = t;
        cache.isValid = true;
    }

    return err;
}

PF_Err InterpolationManager::SmoothCurve(
    CurveData* curve,
    PF_FpLong tension)
{
    PF_Err err = PF_Err_NONE;

    if (curve->num_points < 3) {
        return err;
    }

    // Temporary storage for smoothed points
    std::vector<CurvePoint> smoothedPoints(curve->num_points);

    // Keep endpoints
    smoothedPoints[0] = curve->points[0];
    smoothedPoints[curve->num_points - 1] = curve->points[curve->num_points - 1];

    // Smooth interior points using Catmull-Rom spline
    for (A_long i = 1; i < curve->num_points - 1; i++) {
        PF_FpLong cp1x, cp1y, cp2x, cp2y;
        ERR(CalculateControlPoints(curve, i, tension, &cp1x, &cp1y, &cp2x, &cp2y));

        if (!err) {
            smoothedPoints[i].x = (cp1x + cp2x) / 2;
            smoothedPoints[i].y = (cp1y + cp2y) / 2;
        }
    }

    // Copy back smoothed points
    if (!err) {
        for (A_long i = 0; i < curve->num_points; i++) {
            curve->points[i] = smoothedPoints[i];
        }
        curve->dirty = true;
    }

    return err;
}

PF_Err InterpolationManager::OptimizeCurvePoints(
    CurveData* curve,
    PF_FpLong epsilon)
{
    PF_Err err = PF_Err_NONE;

    if (curve->num_points < 3) {
        return err;
    }

    // Douglas-Peucker algorithm for curve simplification
    std::vector<bool> keepPoint(curve->num_points, false);
    keepPoint[0] = keepPoint[curve->num_points - 1] = true;

    // Find points with maximum deviation
    A_long writeIndex = 1;
    for (A_long i = 1; i < curve->num_points - 1; i++) {
        PF_FpLong dx = curve->points[i + 1].x - curve->points[i - 1].x;
        PF_FpLong dy = curve->points[i + 1].y - curve->points[i - 1].y;
        PF_FpLong d = std::abs(dx * (curve->points[i - 1].y - curve->points[i].y) -
                              (curve->points[i - 1].x - curve->points[i].x) * dy) /
                      std::sqrt(dx * dx + dy * dy);

        if (d > epsilon) {
            keepPoint[i] = true;
            curve->points[writeIndex++] = curve->points[i];
        }
    }

    // Update curve with optimized points
    curve->points[writeIndex++] = curve->points[curve->num_points - 1];
    curve->num_points = writeIndex;
    curve->dirty = true;

    return err;
}

PF_Err InterpolationManager::InterpolatePoints(
    const CurvePoint& p1,
    const CurvePoint& p2,
    PF_FpLong t,
    CurvePoint* result)
{
    // Smooth step interpolation
    PF_FpLong smoothT = t * t * (3 - 2 * t);
    result->x = p1.x + smoothT * (p2.x - p1.x);
    result->y = p1.y + smoothT * (p2.y - p1.y);

    return PF_Err_NONE;
}

PF_Err InterpolationManager::CalculateControlPoints(
    const CurveData* curve,
    A_long index,
    PF_FpLong tension,
    PF_FpLong* cp1x, PF_FpLong* cp1y,
    PF_FpLong* cp2x, PF_FpLong* cp2y)
{
    const CurvePoint& prev = curve->points[index - 1];
    const CurvePoint& curr = curve->points[index];
    const CurvePoint& next = curve->points[index + 1];

    // Calculate control points using Catmull-Rom to Bezier conversion
    PF_FpLong dx = next.x - prev.x;
    PF_FpLong dy = next.y - prev.y;

    *cp1x = curr.x - tension * dx / 3;
    *cp1y = curr.y - tension * dy / 3;
    *cp2x = curr.x + tension * dx / 3;
    *cp2y = curr.y + tension * dy / 3;

    return PF_Err_NONE;
}