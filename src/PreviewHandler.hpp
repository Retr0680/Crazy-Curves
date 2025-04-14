#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class PreviewHandler {
public:
    static PF_Err UpdatePreview(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        const PF_Rect& previewRect);

    static PF_Err DrawRealTimePreview(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const CurveData* curves,
        const PF_Rect& bounds);

private:
    static const A_long PREVIEW_WIDTH = 256;
    static const A_long PREVIEW_HEIGHT = 256;

    static PF_Err ProcessPreviewPixel8(
        const PF_Pixel8* inP,
        PF_Pixel8* outP,
        const CurveData* curves);

    static PF_Err ProcessPreviewPixelFloat(
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP,
        const CurveData* curves);
};