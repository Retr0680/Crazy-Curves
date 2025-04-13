#ifndef MATH_H
#define MATH_H

// Utility functions for mathematical operations

// Function to clamp a value between a minimum and maximum
inline float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

// Function to interpolate between two values
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Function to calculate the power of a number
inline float pow(float base, float exponent) {
    return std::pow(base, exponent);
}

#endif // MATH_H