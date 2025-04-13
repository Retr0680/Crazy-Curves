#include "CurveInterpolation.hpp"
#include <algorithm>

namespace CurveInterpolation {

float CurveInterpolation::interpolate(const std::vector<CurvePoint>& points, float x) {
    if (points.size() < 2) return x;
    
    // Find surrounding points
    auto it = std::lower_bound(points.begin(), points.end(), x,
        [](const CurvePoint& p, float val) { return p.x < val; });
    
    if (it == points.begin()) return points.front().y;
    if (it == points.end()) return points.back().y;
    
    size_t index = std::distance(points.begin(), it);
    float t = (x - points[index-1].x) / (points[index].x - points[index-1].x);
    
    // Get points for cubic interpolation
    float p0 = index > 1 ? points[index-2].y : points[index-1].y;
    float p1 = points[index-1].y;
    float p2 = points[index].y;
    float p3 = index < points.size()-1 ? points[index+1].y : points[index].y;
    
    return CurveInterpolation::cubicInterpolate(p0, p1, p2, p3, t);
}

float CurveInterpolation::cubicInterpolate(float p0, float p1, float p2, float p3, float t) {
    float tension = 0.5f;
    float m1 = CurveInterpolation::getCatmullRomTangent(p0, p1, p2, tension);
    float m2 = CurveInterpolation::getCatmullRomTangent(p1, p2, p3, tension);
    
    float t2 = t * t;
    float t3 = t2 * t;
    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 +
           (t3 - 2.0f * t2 + t) * m1 +
           (-2.0f * t3 + 3.0f * t2) * p2 +
           (t3 - t2) * m2;
}

float CurveInterpolation::getCatmullRomTangent(float p0, float p1, float p2, float tension) {
    return tension * (p2 - p0);
}

}