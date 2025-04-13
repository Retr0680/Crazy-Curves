#pragma once
#include <vector>
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
    void updatePoint(size_t index, float x, float y);
    float evaluate(float x) const;
    void reset();
    
    const std::vector<CurvePoint>& getPoints() const { return points; }
    
private:
    std::vector<CurvePoint> points;
    float interpolate(float x, const CurvePoint& p1, const CurvePoint& p2) const;
};