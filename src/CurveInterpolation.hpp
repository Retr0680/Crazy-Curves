#pragma once

#include <vector>
#include <cmath>
#include "CurvesData.hpp"

// Forward declaration
struct CurvePoint;

namespace CurveInterpolation {
    enum class Method {
        LINEAR,
        CUBIC,
        CATMULL_ROM,
        BEZIER
    };

    class Interpolator {
    public:
        static float interpolate(float x, const CurvePoint& p1, const CurvePoint& p2, 
                               const std::vector<CurvePoint>& points, Method method);
        
    private:
        static float linearInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2);
        static float cubicInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2);
        static float catmullRomInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2,
                                         const CurvePoint* p0, const CurvePoint* p3);
        static float bezierInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2,
                                     const std::vector<CurvePoint>& points);
        static float getCatmullRomTangent(const CurvePoint& p0, const CurvePoint& p1, 
                                        const CurvePoint& p2, const CurvePoint& p3);
    };
}