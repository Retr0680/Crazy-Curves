#include "PresetManager.hpp"

PresetManager::PresetManager() {
    loadDefaultPresets();
}

void PresetManager::loadDefaultPresets() {
    // Linear preset
    CurveData linear;
    linear.addPoint(0.0f, 0.0f);
    linear.addPoint(1.0f, 1.0f);
    savePreset("Linear", linear);

    // S-Curve preset
    CurveData sCurve;
    sCurve.addPoint(0.0f, 0.0f);
    sCurve.addPoint(0.25f, 0.15f);
    sCurve.addPoint(0.5f, 0.5f);
    sCurve.addPoint(0.75f, 0.85f);
    sCurve.addPoint(1.0f, 1.0f);
    savePreset("S-Curve", sCurve);

    // Strong Contrast preset
    CurveData contrast;
    contrast.addPoint(0.0f, 0.0f);
    contrast.addPoint(0.3f, 0.2f);
    contrast.addPoint(0.7f, 0.8f);
    contrast.addPoint(1.0f, 1.0f);
    savePreset("Strong Contrast", contrast);
}

void PresetManager::savePreset(const std::string& name, const CurveData& curve) {
    presets[name] = curve;
    
    // Update preset names list
    if (std::find(presetNames.begin(), presetNames.end(), name) == presetNames.end()) {
        presetNames.push_back(name);
    }
}

bool PresetManager::loadPreset(const std::string& name, CurveData& outCurve) {
    auto it = presets.find(name);
    if (it != presets.end()) {
        outCurve = it->second;
        return true;
    }
    return false;
}

const std::vector<std::string>& PresetManager::getPresetNames() const {
    return presetNames;
}

void PresetManager::deletePreset(const std::string& name) {
    presets.erase(name);
    auto it = std::find(presetNames.begin(), presetNames.end(), name);
    if (it != presetNames.end()) {
        presetNames.erase(it);
    }
}