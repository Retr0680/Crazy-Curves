#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class RenderHandler {
public:
    static PF_Err SmartPreRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_PreRenderExtra* extra);
        
    static PF_Err SmartRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_SmartRenderExtra* extra);

private:
    static PF_Err ProcessPixels8(
        void* refcon,
        A_long x,
        A_long y,
        PF_Pixel8* inP,
        PF_Pixel8* outP);
        
    static PF_Err ProcessPixelsFloat(
        void* refcon,
        A_long x,
        A_long y,
        PF_PixelFloat* inP,
        PF_PixelFloat* outP);
};