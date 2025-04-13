#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class UIEventHandler {
public:
    static PF_Err HandleEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output,
        PF_EventExtra* event);

private:
    static PF_Err HandleMouseDown(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_Point& point);

    static PF_Err HandleDrag(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_Point& point);

    static PF_Err HandleKeyDown(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_KeyDownEvent& key_event);

    static PF_Point GetCurveSpaceCoordinates(const PF_Point& screen_point);
};