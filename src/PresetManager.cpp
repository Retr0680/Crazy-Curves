#include "PresetManager.hpp"

PresetManager::PresetManager() {
    loadDefaultPresets();
}

void PresetManager::loadDefaultPresets() {
    // Linear preset
    CurveData linear;
    linear.reset();
    savePreset("Linear", linear);

    // S-Curve preset
    CurveData sCurve;
    sCurve.reset();
    sCurve.addPoint(0.25f, 0.15f);
    sCurve.addPoint(0.75f, 0.85f);
    savePreset("S-Curve", sCurve);

    // Contrast preset
    CurveData contrast;
    contrast.reset();
    contrast.addPoint(0.25f, 0.1f);
    contrast.addPoint(0.75f, 0.9f);
    savePreset("Contrast", contrast);

    // Negative preset
    CurveData negative;
    negative.reset();
    negative.updatePoint(0, 0.0f, 1.0f);
    negative.updatePoint(1, 1.0f, 0.0f);
    savePreset("Negative", negative);
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