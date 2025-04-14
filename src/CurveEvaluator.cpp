#include "CurveEvaluator.hpp"

PF_Err CurveEvaluator::EvaluatePixel8(
    const CurveData* curves,
    const PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    PF_Err err = PF_Err_NONE;

    // Apply master RGB curve first
    PF_FpLong rgb_r = EvaluateCurve(&curves[0], inP->red / 255.0f);
    PF_FpLong rgb_g = EvaluateCurve(&curves[0], inP->green / 255.0f);
    PF_FpLong rgb_b = EvaluateCurve(&curves[0], inP->blue / 255.0f);

    // Then apply individual channel curves
    outP->red = static_cast<A_u_char>(EvaluateCurve(&curves[1], rgb_r) * 255.0f);
    outP->green = static_cast<A_u_char>(EvaluateCurve(&curves[2], rgb_g) * 255.0f);
    outP->blue = static_cast<A_u_char>(EvaluateCurve(&curves[3], rgb_b) * 255.0f);
    outP->alpha = inP->alpha;

    return err;
}

PF_Err CurveEvaluator::EvaluatePixelFloat(
    const CurveData* curves,
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    PF_Err err = PF_Err_NONE;

    // Apply master RGB curve first
    PF_FpLong rgb_r = EvaluateCurve(&curves[0], inP->red);
    PF_FpLong rgb_g = EvaluateCurve(&curves[0], inP->green);
    PF_FpLong rgb_b = EvaluateCurve(&curves[0], inP->blue);

    // Then apply individual channel curves
    outP->red = static_cast<PF_FpShort>(EvaluateCurve(&curves[1], rgb_r));
    outP->green = static_cast<PF_FpShort>(EvaluateCurve(&curves[2], rgb_g));
    outP->blue = static_cast<PF_FpShort>(EvaluateCurve(&curves[3], rgb_b));
    outP->alpha = inP->alpha;

    return err;
}

PF_FpLong CurveEvaluator::EvaluateCurve(
    const CurveData* curve,
    PF_FpLong x,
    bool useCache)
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
    return InterpolateCurveSegment(curve->points[i], 
                                  curve->points[i + 1], t);
}

PF_FpLong CurveEvaluator::InterpolateCurveSegment(
    const CurvePoint& p1,
    const CurvePoint& p2,
    PF_FpLong t)
{
    // Use smooth step interpolation for better curve quality
    t = t * t * (3.0 - 2.0 * t);
    return p1.y + t * (p2.y - p1.y);
}