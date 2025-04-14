#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class SmartRenderManager {
public:
    static PF_Err PreRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_PreRenderExtra* extra,
        PF_RenderRequest* req);

    static PF_Err SmartRender(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_SmartRenderExtra* extra);

private:
    struct SmartRenderData {
        CurveData curves[4];  // RGB, R, G, B curves
        PF_EffectWorld* input;
        PF_Rect area_to_render;
    };

    static PF_Err ProcessSmartRenderChunk(
        PF_InData* in_data,
        const SmartRenderData* renderData,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const PF_Rect* area);
};