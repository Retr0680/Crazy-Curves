#include "CurveInterpolation.h"

namespace CurveInterpolation {

float Interpolator::interpolate(float x, const CurvePoint& p1, const CurvePoint& p2,
                              const std::vector<CurvePoint>& points, Method method) {
    switch (method) {
        case Method::LINEAR:
            return linearInterpolate(x, p1, p2);
        case Method::CUBIC:
            return cubicInterpolate(x, p1, p2);
        case Method::CATMULL_ROM: {
            // Find points before and after current segment
            const CurvePoint* p0 = nullptr;
            const CurvePoint* p3 = nullptr;
            
            for (size_t i = 0; i < points.size(); ++i) {
                if (points[i].x == p1.x) {
                    if (i > 0) p0 = &points[i-1];
                    if (i < points.size()-2) p3 = &points[i+2];
                    break;
                }
            }
            
            return catmullRomInterpolate(x, p1, p2, p0, p3);
        }
        case Method::BEZIER:
            return bezierInterpolate(x, p1, p2, points);
        default:
            return linearInterpolate(x, p1, p2);
    }
}

float Interpolator::linearInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2) {
    float t = (x - p1.x) / (p2.x - p1.x);
    return p1.y + t * (p2.y - p1.y);
}

float Interpolator::cubicInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2) {
    float t = (x - p1.x) / (p2.x - p1.x);
    float t2 = t * t;
    float t3 = t2 * t;
    
    // Hermite interpolation
    return (2 * t3 - 3 * t2 + 1) * p1.y + (t3 - 2 * t2 + t) * (p2.y - p1.y);
}

float Interpolator::catmullRomInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2,
                                        const CurvePoint* p0, const CurvePoint* p3) {
    float t = (x - p1.x) / (p2.x - p1.x);
    float t2 = t * t;
    float t3 = t2 * t;
    
    // If no points before or after, use current points
    float y0 = p0 ? p0->y : p1.y;
    float y3 = p3 ? p3->y : p2.y;
    
    // Catmull-Rom spline
    float a = -0.5f * y0 + 1.5f * p1.y - 1.5f * p2.y + 0.5f * y3;
    float b = y0 - 2.5f * p1.y + 2.0f * p2.y - 0.5f * y3;
    float c = -0.5f * y0 + 0.5f * p2.y;
    float d = p1.y;
    
    return a * t3 + b * t2 + c * t + d;
}

float Interpolator::bezierInterpolate(float x, const CurvePoint& p1, const CurvePoint& p2,
                                    const std::vector<CurvePoint>& points) {
    // Calculate control points for Bezier curve
    float cp1x = p1.x + (p2.x - p1.x) / 3.0f;
    float cp1y = p1.y + (p2.y - p1.y) / 3.0f;
    float cp2x = p1.x + 2.0f * (p2.x - p1.x) / 3.0f;
    float cp2y = p2.y - (p2.y - p1.y) / 3.0f;
    
    // Find t parameter using Newton's method
    float t = (x - p1.x) / (p2.x - p1.x);
    for (int i = 0; i < 5; ++i) {
        float currentX = p1.x * std::pow(1-t, 3) + 
                        3 * cp1x * t * std::pow(1-t, 2) +
                        3 * cp2x * t * t * (1-t) +
                        p2.x * t * t * t;
        
        float derivative = -3 * p1.x * std::pow(1-t, 2) +
                          3 * cp1x * (1 - 4*t + 3*t*t) +
                          3 * cp2x * (2*t - 3*t*t) +
                          3 * p2.x * t * t;
                          
        t -= (currentX - x) / derivative;
        t = std::max(0.0f, std::min(1.0f, t));
    }
    
    // Calculate final y value
    return p1.y * std::pow(1-t, 3) +
           3 * cp1y * t * std::pow(1-t, 2) +
           3 * cp2y * t * t * (1-t) +
           p2.y * t * t * t;
}

}