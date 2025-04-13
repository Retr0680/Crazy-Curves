#pragma once
#include <string>
#include <vector>
#include <map>
#include "CurveData.hpp"

class PresetManager {
public:
    PresetManager();
    void savePreset(const std::string& name, const CurveData& curve);
    bool loadPreset(const std::string& name, CurveData& outCurve);
    const std::vector<std::string>& getPresetNames() const;
    void deletePreset(const std::string& name);

private:
    std::map<std::string, CurveData> presets;
    std::vector<std::string> presetNames;
    void loadDefaultPresets();
};