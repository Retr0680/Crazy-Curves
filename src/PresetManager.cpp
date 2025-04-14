#include "PresetManager.hpp"
#include "AE_EffectCB.h"
#include "CurvesData.h"
#include <fstream>

PresetManager::PresetManager() {
    initializeDefaultPresets();
}

PF_Err PresetManager::initializeDefaultPresets() {
    PF_Err err = PF_Err_NONE;

    // Create default linear preset
    PresetData linear;
    for (int i = 0; i < 4; i++) {
        linear.curves[i].num_points = 2;
        linear.curves[i].points[0] = {0.0f, 0.0f};
        linear.curves[i].points[1] = {1.0f, 1.0f};
    }
    presets["Linear"] = linear;

    // Create S-curve preset
    PresetData sCurve = linear;
    for (int i = 0; i < 4; i++) {
        sCurve.curves[i].num_points = 4;
        sCurve.curves[i].points[1] = {0.25f, 0.15f};
        sCurve.curves[i].points[2] = {0.75f, 0.85f};
    }
    presets["S-Curve"] = sCurve;

    return err;
}

PF_Err PresetManager::validatePreset(const PresetData& preset) const {
    PF_Err err = PF_Err_NONE;

    for (int i = 0; i < 4; i++) {
        const CurveData& curve = preset.curves[i];
        
        // Check number of points
        if (curve.num_points < 2 || curve.num_points > 256) {
            err = PF_Err_BAD_CALLBACK_PARAM;
            break;
        }

        // Check point ordering
        for (int j = 1; j < curve.num_points; j++) {
            if (curve.points[j].x <= curve.points[j-1].x) {
                err = PF_Err_BAD_CALLBACK_PARAM;
                break;
            }
        }
    }

    return err;
}

PF_Err PresetManager::loadPreset(
    const std::string& name, 
    CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    auto it = presets.find(name);
    if (it != presets.end()) {
        // Copy preset curves to output
        for (int i = 0; i < 4; i++) {
            curves[i] = it->second.curves[i];
            curves[i].dirty = true;
        }
    } else {
        err = PF_Err_INVALID_INDEX;
    }

    return err;
}

PF_Err PresetManager::savePreset(
    const std::string& name,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    PresetData preset;
    for (int i = 0; i < 4; i++) {
        preset.curves[i] = curves[i];
    }
    preset.version = 1;
    preset.timestamp = static_cast<A_long>(time(nullptr));

    ERR(validatePreset(preset));
    
    if (!err) {
        presets[name] = preset;
    }

    return err;
}

PF_Err PresetManager::deletePreset(const std::string& name) {
    PF_Err err = PF_Err_NONE;

    auto it = presets.find(name);
    if (it != presets.end()) {
        presets.erase(it);
    } else {
        err = PF_Err_INVALID_INDEX;
    }

    return err;
}

PF_Err PresetManager::getPresetNames(std::vector<std::string>& names) const {
    names.clear();
    for (const auto& preset : presets) {
        names.push_back(preset.first);
    }
    return PF_Err_NONE;
}