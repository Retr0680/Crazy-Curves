#include "CurveData.hpp"
#include "CurveInterpolation.hpp"

CurveData::CurveData() {
    // Initialize with default curve points
    points.push_back({0.0f, 0.0f});
    points.push_back({1.0f, 1.0f});
}

void CurveData::addPoint(float x, float y) {
    CurvePoint newPoint = {x, y};
    
    // Insert point in sorted order by x coordinate
    auto it = std::lower_bound(points.begin(), points.end(), newPoint,
        [](const CurvePoint& a, const CurvePoint& b) { return a.x < b.x; });
    
    points.insert(it, newPoint);
}

void CurveData::removePoint(size_t index) {
    if (index < points.size() && index > 0 && index < points.size() - 1) {
        points.erase(points.begin() + index);
    }
}

void CurveData::updatePoint(size_t index, float x, float y) {
    if (index < points.size()) {
        if (index == 0) {
            points[index].y = y;  // First point x must stay at 0
        } else if (index == points.size() - 1) {
            points[index].y = y;  // Last point x must stay at 1
        } else {
            points[index].x = x;
            points[index].y = y;
        }
    }
}

float CurveData::interpolate(float x) const {
    return CurveInterpolation::interpolate(points, x);
}

const std::vector<CurvePoint>& CurveData::getPoints() const {
    return points;
}