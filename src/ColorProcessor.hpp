#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include "LUTManager.hpp"

class ColorProcessor {
public:
    static ColorProcessor& getInstance() {
        static ColorProcessor instance;
        return instance;
    }

    PF_Err ProcessFrame(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);

private:
    ColorProcessor() = default;
    
    PF_Err ProcessPixels8(
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const LUTManager& lutManager);
        
    PF_Err ProcessPixelsFloat(
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const LUTManager& lutManager);
};