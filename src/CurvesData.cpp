#include "CurvesData.h"
#include "AE_EffectCB.h"

CurveData::CurveData() {
    // Initialize with default linear curve (diagonal line)
    points.push_back(CurvePoint(0.0f, 0.0f));
    points.push_back(CurvePoint(1.0f, 1.0f));
}

void CurveData::addPoint(float x, float y) {
    // Clamp values between 0 and 1
    x = std::max(0.0f, std::min(1.0f, x));
    y = std::max(0.0f, std::min(1.0f, y));
    
    points.push_back(CurvePoint(x, y));
    sortPoints();
}

void CurveData::removePoint(float x, float y, float tolerance) {
    auto it = std::remove_if(points.begin(), points.end(),
        [x, y, tolerance](const CurvePoint& p) {
            return std::abs(p.x - x) < tolerance && std::abs(p.y - y) < tolerance;
        });
    
    points.erase(it, points.end());
}

float CurveData::evaluate(float x) const {
    if (points.empty()) return x;
    if (points.size() == 1) return points[0].y;
    
    // Find the two points to interpolate between
    for (size_t i = 0; i < points.size() - 1; ++i) {
        if (x >= points[i].x && x <= points[i+1].x) {
            return interpolate(x, points[i], points[i+1]);
        }
    }
    
    // If x is outside the range, clamp to nearest point
    if (x <= points.front().x) return points.front().y;
    return points.back().y;
}

void CurveData::reset() {
    points.clear();
    points.push_back(CurvePoint(0.0f, 0.0f));
    points.push_back(CurvePoint(1.0f, 1.0f));
}

float CurveData::interpolate(float x, const CurvePoint& p1, const CurvePoint& p2) const {
    // Use cubic interpolation for smooth curves
    float t = (x - p1.x) / (p2.x - p1.x);
    t = t * t * (3 - 2 * t); // Smooth step interpolation
    return p1.y + t * (p2.y - p1.y);
}

void CurveData::sortPoints() {
    std::sort(points.begin(), points.end());
}

PF_Err CurvesData::initCurve(CurveData* curve) {
    PF_Err err = PF_Err_NONE;
    
    curve->curve_data_id = CURVEDATA_DISK_ID;
    curve->num_points = 2;
    
    // Initialize with linear curve
    curve->points[0][0] = 0.0;  // x
    curve->points[0][1] = 0.0;  // y
    curve->points[1][0] = 1.0;  // x
    curve->points[1][1] = 1.0;  // y
    
    curve->dirty = true;
    
    return err;
}

PF_Err CurvesData::addPoint(CurveData* curve, PF_FpLong x, PF_FpLong y) {
    PF_Err err = PF_Err_NONE;
    
    if (curve->num_points >= 256) {
        return PF_Err_OUT_OF_MEMORY;
    }
    
    // Insert point maintaining x-order
    int insertIdx = 0;
    for (; insertIdx < curve->num_points; insertIdx++) {
        if (x < curve->points[insertIdx][0]) break;
    }
    
    // Shift existing points
    for (int i = curve->num_points; i > insertIdx; i--) {
        curve->points[i][0] = curve->points[i-1][0];
        curve->points[i][1] = curve->points[i-1][1];
    }
    
    curve->points[insertIdx][0] = x;
    curve->points[insertIdx][1] = y;
    curve->num_points++;
    curve->dirty = true;
    
    return err;
}

PF_FpLong CurvesData::evaluate(const CurveData* curve, PF_FpLong x) {
    if (curve->num_points < 2) return x;
    
    // Find segment containing x
    int i = 0;
    for (; i < curve->num_points - 1; i++) {
        if (x <= curve->points[i+1][0]) break;
    }
    
    // Interpolate between points
    PF_FpLong x0 = curve->points[i][0];
    PF_FpLong y0 = curve->points[i][1];
    PF_FpLong x1 = curve->points[i+1][0];
    PF_FpLong y1 = curve->points[i+1][1];
    
    PF_FpLong t = (x - x0) / (x1 - x0);
    return y0 + t * (y1 - y0);
}

PF_Err CurvesData::reset(CurveData* curve) {
    PF_Err err = PF_Err_NONE;
    
    curve->num_points = 2;
    curve->points[0][0] = 0.0;
    curve->points[0][1] = 0.0;
    curve->points[1][0] = 1.0;
    curve->points[1][1] = 1.0;
    curve->dirty = true;
    
    return err;
}

PF_FpLong CurvesData::interpolate(PF_FpLong x, const CurvePoint& p1, const CurvePoint& p2) {
    PF_FpLong t = (x - p1.x) / (p2.x - p1.x);
    t = t * t * (3 - 2 * t); // Smooth step interpolation
    return p1.y + t * (p2.y - p1.y);
}

void CurvesData::sortPoints(CurveData* curve) {
    // Simple bubble sort since we typically have few points
    for (int i = 0; i < curve->num_points - 1; i++) {
        for (int j = 0; j < curve->num_points - i - 1; j++) {
            if (curve->points[j][0] > curve->points[j+1][0]) {
                // Swap points
                PF_FpLong tempX = curve->points[j][0];
                PF_FpLong tempY = curve->points[j][1];
                curve->points[j][0] = curve->points[j+1][0];
                curve->points[j][1] = curve->points[j+1][1];
                curve->points[j+1][0] = tempX;
                curve->points[j+1][1] = tempY;
            }
        }
    }
}