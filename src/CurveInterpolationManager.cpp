#include "CurveInterpolationManager.hpp"
#include <algorithm>
#include <cmath>

PF_Err CurveInterpolationManager::InterpolateCurve(
    const CurveData* curve1,
    const CurveData* curve2,
    PF_FpLong t,
    CurveData* result,
    InterpolationType type)
{
    PF_Err err = PF_Err_NONE;

    // Initialize result curve
    result->num_points = std::max(curve1->num_points, curve2->num_points);
    result->dirty = true;

    if (type == InterpolationType::Linear) {
        // Simple linear interpolation between curves
        for (A_long i = 0; i < result->num_points; i++) {
            const CurvePoint& p1 = (i < curve1->num_points) ? 
                curve1->points[i] : curve1->points[curve1->num_points - 1];
            const CurvePoint& p2 = (i < curve2->num_points) ? 
                curve2->points[i] : curve2->points[curve2->num_points - 1];

            ERR(InterpolateLinear(p1, p2, t, &result->points[i]));
        }
    } else {
        // Calculate tangents for advanced interpolation
        std::vector<TangentData> tangents1, tangents2;
        ERR(CalculateTangents(curve1, tangents1));
        if (!err) ERR(CalculateTangents(curve2, tangents2));

        if (!err) {
            for (A_long i = 0; i < result->num_points; i++) {
                const CurvePoint& p1 = (i < curve1->num_points) ? 
                    curve1->points[i] : curve1->points[curve1->num_points - 1];
                const CurvePoint& p2 = (i < curve2->num_points) ? 
                    curve2->points[i] : curve2->points[curve2->num_points - 1];

                const TangentData& tang1 = (i < tangents1.size()) ? 
                    tangents1[i] : tangents1.back();
                const TangentData& tang2 = (i < tangents2.size()) ? 
                    tangents2[i] : tangents2.back();

                switch (type) {
                    case InterpolationType::Bezier:
                        ERR(InterpolateBezier(p1, p2, tang1, t, &result->points[i]));
                        break;

                    case InterpolationType::Hermite:
                        ERR(InterpolateHermite(p1, p2, tang1, t, &result->points[i], 0.5, 0.0));
                        break;

                    default: // Smooth or CatmullRom
                        ERR(InterpolateHermite(p1, p2, tang1, t, &result->points[i], 0.0, 0.0));
                        break;
                }
            }
        }
    }

    return err;
}

PF_Err CurveInterpolationManager::SmoothCurve(
    CurveData* curve,
    PF_FpLong tension,
    PF_FpLong bias,
    PF_FpLong continuity)
{
    PF_Err err = PF_Err_NONE;

    if (curve->num_points < 3) return err;

    std::vector<TangentData> tangents;
    ERR(CalculateTangents(curve, tangents));

    if (!err) {
        CurveData tempCurve = *curve;
        
        // Smooth interior points
        for (A_long i = 1; i < curve->num_points - 1; i++) {
            CurvePoint& prev = tempCurve.points[i - 1];
            CurvePoint& curr = tempCurve.points[i];
            CurvePoint& next = tempCurve.points[i + 1];

            // Calculate smoothed point using Kochanek-Bartels spline
            PF_FpLong t = 0.5;
            ERR(InterpolateHermite(prev, next, tangents[i], t, &curr, tension, bias));
        }

        if (!err) {
            *curve = tempCurve;
            curve->dirty = true;
        }
    }

    return err;
}

PF_Err CurveInterpolationManager::OptimizeCurvePoints(
    CurveData* curve,
    PF_FpLong epsilon)
{
    PF_Err err = PF_Err_NONE;

    if (curve->num_points < 3) return err;

    // Douglas-Peucker algorithm
    std::vector<bool> keepPoint(curve->num_points, false);
    keepPoint[0] = keepPoint[curve->num_points - 1] = true;

    // Recursive point reduction
    std::function<void(A_long, A_long)> reducePoints = 
        [&](A_long start, A_long end) {
            if (end - start < 2) return;

            PF_FpLong maxDist = 0;
            A_long maxIndex = start;

            // Find point with maximum deviation
            for (A_long i = start + 1; i < end; i++) {
                PF_FpLong dist = std::abs(
                    (curve->points[i].y - curve->points[start].y) * 
                    (curve->points[end].x - curve->points[start].x) -
                    (curve->points[i].x - curve->points[start].x) * 
                    (curve->points[end].y - curve->points[start].y)
                ) / std::sqrt(
                    std::pow(curve->points[end].x - curve->points[start].x, 2) +
                    std::pow(curve->points[end].y - curve->points[start].y, 2)
                );

                if (dist > maxDist) {
                    maxDist = dist;
                    maxIndex = i;
                }
            }

            if (maxDist > epsilon) {
                keepPoint[maxIndex] = true;
                reducePoints(start, maxIndex);
                reducePoints(maxIndex, end);
            }
        };

    reducePoints(0, curve->num_points - 1);

    // Rebuild curve with kept points
    A_long writeIndex = 0;
    for (A_long i = 0; i < curve->num_points; i++) {
        if (keepPoint[i]) {
            curve->points[writeIndex++] = curve->points[i];
        }
    }
    curve->num_points = writeIndex;
    curve->dirty = true;

    return err;
}

PF_Err CurveInterpolationManager::CalculateTangents(
    const CurveData* curve,
    std::vector<TangentData>& tangents)
{
    PF_Err err = PF_Err_NONE;

    tangents.resize(curve->num_points);

    // Calculate tangents using Catmull-Rom method
    for (A_long i = 0; i < curve->num_points; i++) {
        const CurvePoint& curr = curve->points[i];
        const CurvePoint& prev = (i > 0) ? 
            curve->points[i - 1] : curr;
        const CurvePoint& next = (i < curve->num_points - 1) ? 
            curve->points[i + 1] : curr;

        // Calculate tangent vectors
        tangents[i].outTangentX = (next.x - prev.x) * 0.5;
        tangents[i].outTangentY = (next.y - prev.y) * 0.5;
        tangents[i].inTangentX = -tangents[i].outTangentX;
        tangents[i].inTangentY = -tangents[i].outTangentY;
    }

    return err;
}

PF_Err CurveInterpolationManager::InterpolateLinear(
    const CurvePoint& p1,
    const CurvePoint& p2,
    PF_FpLong t,
    CurvePoint* result)
{
    result->x = p1.x + t * (p2.x - p1.x);
    result->y = p1.y + t * (p2.y - p1.y);
    return PF_Err_NONE;
}

PF_Err CurveInterpolationManager::InterpolateBezier(
    const CurvePoint& p1,
    const CurvePoint& p2,
    const TangentData& tangents,
    PF_FpLong t,
    CurvePoint* result)
{
    // Cubic Bezier interpolation
    PF_FpLong t2 = t * t;
    PF_FpLong t3 = t2 * t;
    PF_FpLong mt = 1 - t;
    PF_FpLong mt2 = mt * mt;
    PF_FpLong mt3 = mt2 * mt;

    result->x = mt3 * p1.x + 
                3 * mt2 * t * (p1.x + tangents.outTangentX) +
                3 * mt * t2 * (p2.x + tangents.inTangentX) +
                t3 * p2.x;

    result->y = mt3 * p1.y +
                3 * mt2 * t * (p1.y + tangents.outTangentY) +
                3 * mt * t2 * (p2.y + tangents.inTangentY) +
                t3 * p2.y;

    return PF_Err_NONE;
}

PF_Err CurveInterpolationManager::InterpolateHermite(
    const CurvePoint& p1,
    const CurvePoint& p2,
    const TangentData& tangents,
    PF_FpLong t,
    CurvePoint* result,
    PF_FpLong tension,
    PF_FpLong bias)
{
    // Hermite interpolation with tension and bias
    PF_FpLong t2 = t * t;
    PF_FpLong t3 = t2 * t;
    
    // Hermite basis functions
    PF_FpLong h1 = 2 * t3 - 3 * t2 + 1;
    PF_FpLong h2 = -2 * t3 + 3 * t2;
    PF_FpLong h3 = t3 - 2 * t2 + t;
    PF_FpLong h4 = t3 - t2;

    // Apply tension and bias
    PF_FpLong scale = (1 - tension) * (1 - std::abs(bias));
    PF_FpLong inScale = scale * (1 - bias);
    PF_FpLong outScale = scale * (1 + bias);

    result->x = h1 * p1.x + 
                h2 * p2.x + 
                h3 * (tangents.outTangentX * outScale) + 
                h4 * (tangents.inTangentX * inScale);

    result->y = h1 * p1.y + 
                h2 * p2.y + 
                h3 * (tangents.outTangentY * outScale) + 
                h4 * (tangents.inTangentY * inScale);

    return PF_Err_NONE;
}