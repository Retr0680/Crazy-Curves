#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class RenderUtils {
public:
    static PF_Err PreRenderSetup(
        PF_InData* in_data,
        PF_RenderRequest* req,
        PF_CheckoutResult* in_result);

    static PF_Err RenderCurves(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves,
        const AEGP_SuiteHandler& suites);

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