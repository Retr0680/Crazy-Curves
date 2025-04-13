#pragma once
#include <vector>
#include "CurveData.hpp"

class CurveInterpolation {
public:
    static float interpolate(const std::vector<CurvePoint>& points, float x);
    
private:
    static float cubicInterpolate(float p0, float p1, float p2, float p3, float t);
    static float getCatmullRomTangent(float p0, float p1, float p2, float tension);
};