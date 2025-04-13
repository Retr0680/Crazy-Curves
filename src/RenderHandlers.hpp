#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class RenderHandlers {
public:
    static PF_Err PreRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_PreRenderExtra* extra);

    static PF_Err SmartRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_SmartRenderExtra* extra);

private:
    static PF_Err ProcessPixel8(
        void* refcon,
        A_long xL,
        A_long yL,
        PF_Pixel8* inP,
        PF_Pixel8* outP);

    static PF_Err ProcessPixelFloat(
        void* refcon,
        A_long xL,
        A_long yL,
        PF_PixelFloat* inP,
        PF_PixelFloat* outP);
};