#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ImageHandler {
public:
    static PF_Err ProcessImage8(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);

    static PF_Err ProcessImageFloat(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);

private:
    static PF_Err ProcessPixel8(
        void* refcon,
        A_long x,
        A_long y,
        PF_Pixel8* inP,
        PF_Pixel8* outP);

    static PF_Err ProcessPixelFloat(
        void* refcon,
        A_long x,
        A_long y,
        PF_PixelFloat* inP,
        PF_PixelFloat* outP);
};