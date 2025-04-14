#pragma once 
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class RealtimePreviewManager {
public:
    static RealtimePreviewManager& getInstance() {
        static RealtimePreviewManager instance;
        return instance;
    }

    PF_Err UpdatePreview(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        const CurveData* curves);

    PF_Err DrawPreviewGrid(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds);

    PF_Err ProcessPreviewArea(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const PF_Rect& area,
        const CurveData* curves);

private:
    static const A_long PREVIEW_SIZE = 256;
    static const A_long GRID_DIVISIONS = 8;
    
    PF_Err CreatePreviewWorld(
        PF_InData* in_data,
        PF_EffectWorld** world);

    PF_Err ProcessPreviewPixel8(
        const PF_Pixel8* inP,
        PF_Pixel8* outP,
        const CurveData* curves);

    PF_Err ProcessPreviewPixelFloat(
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP,
        const CurveData* curves);
};