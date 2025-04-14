#include "BezierCurveHandler.hpp"

PF_Err BezierCurveHandler::EvaluateBezier(
    const CurvePoint& p0,
    const CurvePoint& p1,
    const CurvePoint& p2,
    const CurvePoint& p3,
    PF_FpLong t,
    PF_FpLong* result)
{
    PF_Err err = PF_Err_NONE;

    // Clamp t to [0,1]
    t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);

    // Calculate Bernstein polynomials
    PF_FpLong b0 = BernsteinPolynomial(0, t);
    PF_FpLong b1 = BernsteinPolynomial(1, t);
    PF_FpLong b2 = BernsteinPolynomial(2, t);
    PF_FpLong b3 = BernsteinPolynomial(3, t);

    // Calculate point on curve
    *result = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;

    return err;
}

PF_Err BezierCurveHandler::CalculateControlPoints(
    const CurveData* curve,
    A_long index,
    PF_FpLong tension,
    PF_FpLong* cp1x, PF_FpLong* cp1y,
    PF_FpLong* cp2x, PF_FpLong* cp2y)
{
    PF_Err err = PF_Err_NONE;

    if (index <= 0 || index >= curve->num_points - 1) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    const CurvePoint& prev = curve->points[index - 1];
    const CurvePoint& curr = curve->points[index];
    const CurvePoint& next = curve->points[index + 1];

    // Calculate control points using Catmull-Rom spline to Bezier conversion
    PF_FpLong dx = next.x - prev.x;
    PF_FpLong dy = next.y - prev.y;

    *cp1x = curr.x - tension * dx / 3;
    *cp1y = curr.y - tension * dy / 3;
    *cp2x = curr.x + tension * dx / 3;
    *cp2y = curr.y + tension * dy / 3;

    return err;
}

PF_Err BezierCurveHandler::SmoothCurve(
    CurveData* curve,
    PF_FpLong smoothness)
{
    PF_Err err = PF_Err_NONE;

    if (!curve || curve->num_points < 3) {
        return PF_Err_NONE;
    }

    // Temporary storage for smoothed points
    CurvePoint* temp = new CurvePoint[curve->num_points];
    if (!temp) return PF_Err_OUT_OF_MEMORY;

    // Keep endpoints
    temp[0] = curve->points[0];
    temp[curve->num_points - 1] = curve->points[curve->num_points - 1];

    // Smooth interior points
    for (A_long i = 1; i < curve->num_points - 1; i++) {
        PF_FpLong cp1x, cp1y, cp2x, cp2y;
        ERR(CalculateControlPoints(curve, i, smoothness, &cp1x, &cp1y, &cp2x, &cp2y));

        if (!err) {
            temp[i].x = (cp1x + cp2x) / 2;
            temp[i].y = (cp1y + cp2y) / 2;
        }
    }

    // Copy back smoothed points
    if (!err) {
        for (A_long i = 0; i < curve->num_points; i++) {
            curve->points[i] = temp[i];
        }
        curve->dirty = true;
    }

    delete[] temp;
    return err;
}

PF_FpLong BezierCurveHandler::BernsteinPolynomial(int i, PF_FpLong t)
{
    PF_FpLong t1 = 1.0 - t;
    
    switch(i) {
        case 0: return t1 * t1 * t1;
        case 1: return 3 * t * t1 * t1;
        case 2: return 3 * t * t * t1;
        case 3: return t * t * t;
        default: return 0;
    }
}