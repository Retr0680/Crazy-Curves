#include "CurvePresets.h"

std::map<std::string, CurveData> CurvePresets::customPresets;

CurveData CurvePresets::getPreset(PresetType type) {
    CurveData curve;
    
    switch (type) {
        case PresetType::LINEAR:
            curve.reset();
            break;
            
        case PresetType::S_CURVE:
            return createSCurve();
            
        case PresetType::CONTRAST:
            return createContrastCurve();
            
        case PresetType::NEGATIVE:
            return createNegativeCurve();
            
        case PresetType::SOLARIZE:
            return createSolarizeCurve();
            
        case PresetType::VINTAGE:
            return createVintageCurve();
            
        default:
            curve.reset();
    }
    
    return curve;
}

CurveData CurvePresets::createSCurve() {
    CurveData curve;
    curve.reset();
    
    // Create classic S-curve for increased contrast
    curve.addPoint(0.25f, 0.15f);
    curve.addPoint(0.75f, 0.85f);
    
    return curve;
}

CurveData CurvePresets::createContrastCurve() {
    CurveData curve;
    curve.reset();
    
    // Create high contrast curve
    curve.addPoint(0.25f, 0.1f);
    curve.addPoint(0.5f, 0.5f);
    curve.addPoint(0.75f, 0.9f);
    
    return curve;
}

CurveData CurvePresets::createNegativeCurve() {
    CurveData curve;
    curve.reset();
    
    // Create negative curve (inverted)
    curve.addPoint(0.0f, 1.0f);
    curve.addPoint(1.0f, 0.0f);
    
    return curve;
}

CurveData CurvePresets::createSolarizeCurve() {
    CurveData curve;
    curve.reset();
    
    // Create solarization effect
    curve.addPoint(0.25f, 0.75f);
    curve.addPoint(0.5f, 0.5f);
    curve.addPoint(0.75f, 0.25f);
    
    return curve;
}

CurveData CurvePresets::createVintageCurve() {
    CurveData curve;
    curve.reset();
    
    // Create vintage look with slightly raised shadows and lowered highlights
    curve.addPoint(0.2f, 0.25f);
    curve.addPoint(0.4f, 0.45f);
    curve.addPoint(0.6f, 0.55f);
    curve.addPoint(0.8f, 0.75f);
    
    return curve;
}

void CurvePresets::saveCustomPreset(const std::string& name, const CurveData& curve) {
    customPresets[name] = curve;
}

std::vector<std::string> CurvePresets::getCustomPresetNames() {
    std::vector<std::string> names;
    for (const auto& preset : customPresets) {
        names.push_back(preset.first);
    }
    return names;
}