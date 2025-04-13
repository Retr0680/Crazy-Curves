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
    PF_Err ProcessPixel8(const PF_Pixel8* inP, PF_Pixel8* outP) const;
    PF_Err ProcessPixelFloat(const PF_PixelFloat* inP, PF_PixelFloat* outP) const;
    void InvalidateLUTs();
    bool AreLUTsValid() const { return lutsValid; }

private:
    LUTManager() : lutsValid(false) {}
    
    static const A_long LUT_SIZE = 256;
    static const A_long NUM_CHANNELS = 4; // RGB + Master

    PF_FpLong lutTables[NUM_CHANNELS][LUT_SIZE];
    bool lutsValid;

    PF_Err BuildLUT(const CurveData* curve, PF_FpLong* table);
    PF_Err InterpolateLUT(PF_FpLong input, const PF_FpLong* table, PF_FpLong* output) const;
};