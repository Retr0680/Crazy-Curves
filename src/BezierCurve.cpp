#include "BezierCurve.hpp"
#include <cmath>

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