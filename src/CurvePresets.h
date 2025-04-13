#pragma once

#include <string>
#include <map>
#include <vector>
#include "CurvesData.h"

class CurvePresets {
public:
    enum class PresetType {
        LINEAR,
        S_CURVE,
        CONTRAST,
        NEGATIVE,
        SOLARIZE,
        VINTAGE,
        CUSTOM
    };
    
    static CurveData getPreset(PresetType type);
    static void saveCustomPreset(const std::string& name, const CurveData& curve);
    static std::vector<std::string> getCustomPresetNames();
    
private:
    static std::map<std::string, CurveData> customPresets;
    
    static CurveData createSCurve();
    static CurveData createContrastCurve();
    static CurveData createNegativeCurve();
    static CurveData createSolarizeCurve();
    static CurveData createVintageCurve();
};