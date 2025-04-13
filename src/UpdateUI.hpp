#pragma once
#include "AE_Effect.h"
#include "AE_EffectUI.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class UpdateUI {
public:
    static PF_Err Draw(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output);

    static PF_Err HandleMouseDown(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_EventExtra* event);

private:
    static PF_Err DrawCurveEditor(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve);

    static PF_Err DrawPoints(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve);
};