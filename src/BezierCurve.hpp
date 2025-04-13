#pragma once
#include <vector>
#include "CurveData.hpp"

struct BezierPoint {
    float x, y;           // Anchor point
    float cx1, cy1;       // Control point 1 (before anchor)
    float cx2, cy2;       // Control point 2 (after anchor)
    bool selected;
    bool controlPoint1Selected;
    bool controlPoint2Selected;
};

class BezierCurve {
public:
    BezierCurve();
    void addPoint(float x, float y);
    void updatePoint(size_t index, float x, float y);
    void updateControlPoint(size_t index, bool isFirstControl, float x, float y);
    float evaluate(float t) const;
    const std::vector<BezierPoint>& getPoints() const { return points; }

private:
    std::vector<BezierPoint> points;
    float evaluateSegment(const BezierPoint& p1, const BezierPoint& p2, float t) const;
};