#include "math.h"
#include <cmath>

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