#pragma once
#include <vector>
#include "AE_Effect.h"

struct CurvePoint {
    float x;
    float y;
};

class CurveData {
public:
    CurveData();
    void addPoint(float x, float y);
    void removePoint(size_t index);
    void updatePoint(size_t index, float x, float y);
    float interpolate(float x) const;
    const std::vector<CurvePoint>& getPoints() const;
    
private:
    std::vector<CurvePoint> points;
};