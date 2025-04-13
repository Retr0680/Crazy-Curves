#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include "CurveInterpolation.hpp"

struct CurvePoint {
    float x;
    float y;
    
    CurvePoint(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    
    bool operator<(const CurvePoint& other) const {
        return x < other.x;
    }
};

class CurveData {
public:
    CurveData();
    void addPoint(float x, float y);
    void removePoint(float x, float y, float tolerance = 0.01f);
    float evaluate(float x) const;
    void reset();
    
    const std::vector<CurvePoint>& getPoints() const { return points; }
    void setInterpolationMethod(CurveInterpolation::Method method);

private:
    std::vector<CurvePoint> points;
    CurveInterpolation::Method interpolationMethod;
    void sortPoints();
};