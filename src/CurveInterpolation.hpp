#pragma once

#include <vector>
#include "CurvesData.hpp"

namespace CurveInterpolation {
    enum class Method {
        LINEAR,
        CUBIC,
        CATMULL_ROM,
        BEZIER
    };

    float interpolate(const std::vector<CurvePoint>& points, float x);
    float cubicInterpolate(float p0, float p1, float p2, float p3, float t);
    float getCatmullRomTangent(float p0, float p1, float p2, float tension);
}