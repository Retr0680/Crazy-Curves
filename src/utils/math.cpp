#include "math.h"
#include <cmath>

namespace MathUtils {
    PF_FpLong lerp(PF_FpLong a, PF_FpLong b, PF_FpLong t) {
        return a + t * (b - a);
    }
    
    PF_FpLong clamp(PF_FpLong value, PF_FpLong min, PF_FpLong max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
}

// Function to interpolate between two values
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Function to apply a curve adjustment based on control points
float applyCurve(float input, const std::vector<std::pair<float, float>>& controlPoints) {
    // Simple linear interpolation for demonstration
    if (controlPoints.empty()) return input;

    float output = input;
    for (size_t i = 1; i < controlPoints.size(); ++i) {
        if (input < controlPoints[i].first) {
            float t = (input - controlPoints[i - 1].first) / (controlPoints[i].first - controlPoints[i - 1].first);
            output = lerp(controlPoints[i - 1].second, controlPoints[i].second, t);
            break;
        }
    }
    return output;
}