#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class LUTManager {
public:
    static LUTManager& getInstance() {
        static LUTManager instance;
        return instance;
    }

    PF_Err InitializeLUTs(const CurveData* curves);
    PF_Err UpdateLUTs(const CurveData* curves);
    void InvalidateLUTs();

    PF_FpLong EvaluateRGB(PF_FpLong input) const;
    PF_FpLong EvaluateRed(PF_FpLong input) const;
    PF_FpLong EvaluateGreen(PF_FpLong input) const;
    PF_FpLong EvaluateBlue(PF_FpLong input) const;

private:
    static const A_long LUT_SIZE = 256;
    
    struct LUTData {
        PF_FpLong values[LUT_SIZE];
        bool isValid;
    };

    LUTData rgbLUT;
    LUTData redLUT;
    LUTData greenLUT;
    LUTData blueLUT;

    PF_Err BuildLUT(
        const CurveData* curve,
        LUTData& lut);

    PF_FpLong InterpolateLUT(
        const LUTData& lut,
        PF_FpLong input) const;
};