#include "curves.h"
#include <algorithm>

// Constructor for the Curves class
Curves::Curves() {
    // Initialize curve parameters
}

// Method to apply curve adjustments to a layer
void Curves::applyCurves(Layer& layer, const CurveData& curveData) {
    // Logic to apply curve adjustments to the specified layer
}

// Method to set curve data
void Curves::setCurveData(const CurveData& curveData) {
    this->curveData = curveData;
}

// Method to get curve data
CurveData Curves::getCurveData() const {
    return curveData;
}

PF_Err CurvesEffect::interpolateCurve(const CurveDef& curve, PF_FpLong input, PF_FpLong& output) {
    PF_Err err = PF_Err_NONE;
    
    // Find the segment containing the input value
    int i = 0;
    for (; i < curve.numPoints - 1; i++) {
        if (input >= curve.points[i].x && input <= curve.points[i + 1].x) {
            break;
        }
    }
    
    // Linear interpolation between control points
    PF_FpLong t = (input - curve.points[i].x) / (curve.points[i + 1].x - curve.points[i].x);
    output = curve.points[i].y + t * (curve.points[i + 1].y - curve.points[i].y);
    
    return err;
}

PF_Err CurvesEffect::applyCurves(CrazyCurvesParams* params, PF_Pixel8* input, PF_Pixel8* output) {
    PF_Err err = PF_Err_NONE;
    
    PF_FpLong r = input->red / 255.0;
    PF_FpLong g = input->green / 255.0;
    PF_FpLong b = input->blue / 255.0;
    
    PF_FpLong newR, newG, newB;
    
    // Apply master curve
    interpolateCurve(params->masterCurve, r, newR);
    interpolateCurve(params->masterCurve, g, newG);
    interpolateCurve(params->masterCurve, b, newB);
    
    // Apply individual channel curves
    interpolateCurve(params->redCurve, newR, newR);
    interpolateCurve(params->greenCurve, newG, newG);
    interpolateCurve(params->blueCurve, newB, newB);
    
    // Clamp values between 0 and 1
    output->red = static_cast<A_u_char>(std::clamp(newR * 255.0, 0.0, 255.0));
    output->green = static_cast<A_u_char>(std::clamp(newG * 255.0, 0.0, 255.0));
    output->blue = static_cast<A_u_char>(std::clamp(newB * 255.0, 0.0, 255.0));
    output->alpha = input->alpha;
    
    return err;
}