#include "BezierCurve.hpp"
#include <cmath>
#include <algorithm>

BezierCurve::BezierCurve() {
    // Initialize with default end points
    BezierPoint start = {0, 0, 0.33f, 0, 0.66f, 0, false, false, false};
    BezierPoint end = {1, 1, 0.33f, 1, 0.66f, 1, false, false, false};
    points.push_back(start);
    points.push_back(end);
}

void BezierCurve::addPoint(float x, float y) {
    BezierPoint newPoint;
    newPoint.x = x;
    newPoint.y = y;
    newPoint.cx1 = x - 0.1f;
    newPoint.cy1 = y;
    newPoint.cx2 = x + 0.1f;
    newPoint.cy2 = y;
    newPoint.selected = false;
    newPoint.controlPoint1Selected = false;
    newPoint.controlPoint2Selected = false;

    // Insert point in sorted order
    auto it = std::lower_bound(points.begin(), points.end(), x,
        [](const BezierPoint& p, float val) { return p.x < val; });
    points.insert(it, newPoint);
}

void BezierCurve::updatePoint(size_t index, float x, float y) {
    if (index >= points.size()) return;

    float dx = x - points[index].x;
    float dy = y - points[index].y;

    // Move control points with anchor point
    points[index].cx1 += dx;
    points[index].cy1 += dy;
    points[index].cx2 += dx;
    points[index].cy2 += dy;
    points[index].x = x;
    points[index].y = y;
}

void BezierCurve::updateControlPoint(size_t index, bool isFirstControl, float x, float y) {
    if (index >= points.size()) return;

    if (isFirstControl) {
        points[index].cx1 = x;
        points[index].cy1 = y;
    } else {
        points[index].cx2 = x;
        points[index].cy2 = y;
    }
}

float BezierCurve::evaluate(float t) const {
    if (points.size() < 2) return t;
    
    // Find the segment containing t
    size_t segmentIndex = 0;
    float segmentT = t;
    
    for (size_t i = 0; i < points.size() - 1; i++) {
        float segmentStart = points[i].x;
        float segmentEnd = points[i + 1].x;
        
        if (t >= segmentStart && t <= segmentEnd) {
            segmentIndex = i;
            segmentT = (t - segmentStart) / (segmentEnd - segmentStart);
            break;
        }
    }
    
    return evaluateSegment(points[segmentIndex], points[segmentIndex + 1], segmentT);
}

float BezierCurve::evaluateSegment(const BezierPoint& p1, const BezierPoint& p2, float t) const {
    float t2 = t * t;
    float t3 = t2 * t;
    float mt = 1 - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    
    return mt3 * p1.y + 
           3 * mt2 * t * p1.cy2 + 
           3 * mt * t2 * p2.cy1 + 
           t3 * p2.y;
}

PF_Err BezierCurve::Initialize(BezierCurveData* curve) {
    PF_Err err = PF_Err_NONE;
    
    curve->curve_id = 1;
    curve->num_points = 2;
    
    // Initialize with linear curve
    curve->points[0] = {0.0, 0.0, 0.33, 0.33, 0.66, 0.66};
    curve->points[1] = {1.0, 1.0, 0.33, 0.33, 0.66, 0.66};
    
    curve->dirty = true;
    
    return err;
}

PF_Err BezierCurve::AddPoint(BezierCurveData* curve, PF_FpLong x, PF_FpLong y) {
    if (curve->num_points >= 256) {
        return PF_Err_OUT_OF_MEMORY;
    }

    A_long index = curve->num_points++;
    curve->points[index].x = x;
    curve->points[index].y = y;
    
    // Set default control points at 1/3 and 2/3 distances
    PF_FpLong prev_x = index > 0 ? curve->points[index-1].x : x;
    PF_FpLong prev_y = index > 0 ? curve->points[index-1].y : y;
    
    curve->points[index].cp1x = prev_x + (x - prev_x) * 0.33;
    curve->points[index].cp1y = prev_y + (y - prev_y) * 0.33;
    curve->points[index].cp2x = prev_x + (x - prev_x) * 0.66;
    curve->points[index].cp2y = prev_y + (y - prev_y) * 0.66;
    
    curve->dirty = true;
    return PF_Err_NONE;
}

PF_Err BezierCurve::SetControlPoints(BezierCurveData* curve, A_long index,
                                    PF_FpLong cp1x, PF_FpLong cp1y,
                                    PF_FpLong cp2x, PF_FpLong cp2y) {
    if (index >= curve->num_points) {
        return PF_Err_BAD_INDEX;
    }
    
    curve->points[index].cp1x = cp1x;
    curve->points[index].cp1y = cp1y;
    curve->points[index].cp2x = cp2x;
    curve->points[index].cp2y = cp2y;
    
    curve->dirty = true;
    return PF_Err_NONE;
}

PF_FpLong BezierCurve::Evaluate(const BezierCurveData* curve, PF_FpLong t) {
    if (curve->num_points < 2) return t;
    
    // Find segment containing t
    A_long i = 0;
    for (; i < curve->num_points - 1; i++) {
        if (t <= curve->points[i + 1].x) break;
    }
    
    // Normalize t to segment
    PF_FpLong local_t = (t - curve->points[i].x) / 
                        (curve->points[i + 1].x - curve->points[i].x);
    
    return BezierInterpolate(curve->points[i], curve->points[i + 1], local_t);
}

PF_FpLong BezierCurve::BezierInterpolate(const BezierPoint& p1, const BezierPoint& p2, PF_FpLong t) {
    PF_FpLong t2 = t * t;
    PF_FpLong t3 = t2 * t;
    PF_FpLong mt = 1 - t;
    PF_FpLong mt2 = mt * mt;
    PF_FpLong mt3 = mt2 * mt;
    
    return mt3 * p1.y +
           3.0 * mt2 * t * p1.cp2y +
           3.0 * mt * t2 * p2.cp1y +
           t3 * p2.y;
}

PF_Err BezierCurve::Reset(BezierCurveData* curve) {
    return Initialize(curve);
}