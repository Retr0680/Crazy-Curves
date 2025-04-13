#include "CurvesData.h"

CurveData::CurveData() {
    // Initialize with default linear curve (diagonal line)
    points.push_back(CurvePoint(0.0f, 0.0f));
    points.push_back(CurvePoint(1.0f, 1.0f));
}

void CurveData::addPoint(float x, float y) {
    // Clamp values between 0 and 1
    x = std::max(0.0f, std::min(1.0f, x));
    y = std::max(0.0f, std::min(1.0f, y));
    
    points.push_back(CurvePoint(x, y));
    sortPoints();
}

void CurveData::removePoint(float x, float y, float tolerance) {
    auto it = std::remove_if(points.begin(), points.end(),
        [x, y, tolerance](const CurvePoint& p) {
            return std::abs(p.x - x) < tolerance && std::abs(p.y - y) < tolerance;
        });
    
    points.erase(it, points.end());
}

float CurveData::evaluate(float x) const {
    if (points.empty()) return x;
    if (points.size() == 1) return points[0].y;
    
    // Find the two points to interpolate between
    for (size_t i = 0; i < points.size() - 1; ++i) {
        if (x >= points[i].x && x <= points[i+1].x) {
            return interpolate(x, points[i], points[i+1]);
        }
    }
    
    // If x is outside the range, clamp to nearest point
    if (x <= points.front().x) return points.front().y;
    return points.back().y;
}

void CurveData::reset() {
    points.clear();
    points.push_back(CurvePoint(0.0f, 0.0f));
    points.push_back(CurvePoint(1.0f, 1.0f));
}

float CurveData::interpolate(float x, const CurvePoint& p1, const CurvePoint& p2) const {
    // Use cubic interpolation for smooth curves
    float t = (x - p1.x) / (p2.x - p1.x);
    t = t * t * (3 - 2 * t); // Smooth step interpolation
    return p1.y + t * (p2.y - p1.y);
}

void CurveData::sortPoints() {
    std::sort(points.begin(), points.end());
}