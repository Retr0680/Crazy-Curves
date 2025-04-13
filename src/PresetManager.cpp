#include "PresetManager.hpp"
#include "AE_EffectCB.h"
#include "CurvesData.h"

PresetManager::PresetManager() {
    loadDefaultPresets();
}

void PresetManager::loadDefaultPresets() {
    // Linear (default)
    CurveData linear;
    CurvesData::initCurve(&linear);
    presets["Linear"] = linear;
    presetNames.push_back("Linear");

    // S-Curve
    CurveData sCurve;
    CurvesData::initCurve(&sCurve);
    CurvesData::addPoint(&sCurve, 0.25f, 0.15f);
    CurvesData::addPoint(&sCurve, 0.75f, 0.85f);
    presets["S-Curve"] = sCurve;
    presetNames.push_back("S-Curve");

    // Contrast
    CurveData contrast;
    CurvesData::initCurve(&contrast);
    CurvesData::addPoint(&contrast, 0.25f, 0.1f);
    CurvesData::addPoint(&contrast, 0.5f, 0.5f);
    CurvesData::addPoint(&contrast, 0.75f, 0.9f);
    presets["Contrast"] = contrast;
    presetNames.push_back("Contrast");

    // Negative
    CurveData negative;
    CurvesData::initCurve(&negative);
    negative.points[0].y = 1.0f;
    negative.points[1].y = 0.0f;
    presets["Negative"] = negative;
    presetNames.push_back("Negative");
}

void PresetManager::savePreset(const std::string& name, const CurveData& curve) {
    presets[name] = curve;
    
    // Update preset names list if new
    if (std::find(presetNames.begin(), presetNames.end(), name) == std::end(presetNames)) {
        presetNames.push_back(name);
    }
}

PF_Err PresetManager::applyPreset(const std::string& name, CurveData* curve) {
    PF_Err err = PF_Err_NONE;
    
    auto it = presets.find(name);
    if (it != presets.end()) {
        // Copy preset curve data
        curve->curve_id = it->second.curve_id;
        curve->num_points = it->second.num_points;
        
        for (A_long i = 0; i < curve->num_points; i++) {
            curve->points[i] = it->second.points[i];
        }
        
        curve->dirty = true;
    } else {
        err = PF_Err_INVALID_INDEX;
    }
    
    return err;
}

void PresetManager::deletePreset(const std::string& name) {
    presets.erase(name);
    auto it = std::find(presetNames.begin(), presetNames.end(), name);
    if (it != presetNames.end()) {
        presetNames.erase(it);
    }
}

const std::vector<std::string>& PresetManager::getPresetNames() const {
    return presetNames;
}