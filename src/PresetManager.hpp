#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>
#include <map>
#include <vector>

class PresetManager {
public:
    static PresetManager& getInstance() {
        static PresetManager instance;
        return instance;
    }

    PF_Err loadPreset(const std::string& name, CurveData* curves);
    PF_Err savePreset(const std::string& name, const CurveData* curves);
    PF_Err deletePreset(const std::string& name);
    PF_Err getPresetNames(std::vector<std::string>& names) const;
    PF_Err importPresets(const char* filePath);
    PF_Err exportPresets(const char* filePath) const;

private:
    PresetManager();
    
    struct PresetData {
        CurveData curves[4];  // RGB, R, G, B curves
        A_char version;
        A_long timestamp;
    };

    std::map<std::string, PresetData> presets;
    
    PF_Err initializeDefaultPresets();
    PF_Err validatePreset(const PresetData& preset) const;
    PF_Err serializePreset(const PresetData& preset, void* buffer, size_t& size) const;
    PF_Err deserializePreset(PresetData& preset, const void* buffer, size_t size);
};