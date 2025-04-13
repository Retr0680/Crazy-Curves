#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

PF_Err Render(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output);

PF_Err ProcessPixel8(
    void* refcon,
    A_long x,
    A_long y,
    PF_Pixel8* inP,
    PF_Pixel8* outP);

PF_Err ProcessPixelFloat(
    void* refcon,
    A_long x,
    A_long y,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP);