#include "CurveData.hpp"
#include <algorithm>

CurveData::CurveData() {
    // Initialize with default end points
    points.emplace_back(0.0f, 0.0f);
    points.emplace_back(1.0f, 1.0f);
}

void CurveData::addPoint(float x, float y) {
    CurvePoint newPoint(x, y);
    
    auto it = std::lower_bound(points.begin(), points.end(), newPoint);
    points.insert(it, newPoint);
}

void CurveData::removePoint(float x, float y, float tolerance) {
    auto it = std::remove_if(points.begin(), points.end(),
        [x, y, tolerance](const CurvePoint& p) {
            return std::abs(p.x - x) < tolerance && std::abs(p.y - y) < tolerance;
        });
    points.erase(it, points.end());
}

void CurveData::updatePoint(size_t index, float x, float y) {
    if (index < points.size()) {
        points[index].x = x;
        points[index].y = y;
        
        // Maintain sorted order
        std::sort(points.begin(), points.end());
    }
}

float CurveData::evaluate(float x) const {
    return CurveInterpolation::interpolate(points, x);
}

void CurveData::reset() {
    points.clear();
    points.emplace_back(0.0f, 0.0f);
    points.emplace_back(1.0f, 1.0f);
}