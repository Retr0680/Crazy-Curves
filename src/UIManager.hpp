#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class UIManager {
public:
    static UIManager& getInstance() {
        static UIManager instance;
        return instance;
    }

    PF_Err DrawUI(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output);
        
    PF_Err HandleUIEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output,
        PF_EventExtra* event);

private:
    UIManager() = default;

    // UI Element Drawing
    PF_Err DrawCurveEditor(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve);

    PF_Err DrawChannelSelectors(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Point& origin);

    PF_Err DrawControls(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Point& origin);

    // Event Handling
    PF_Err HandleMouseDown(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_Point& point);

    PF_Err HandleDrag(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_Point& point);

    PF_Err HandleKeyDown(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_KeyDownEvent& event);

    // UI Helpers
    PF_Point GetCurveCoordinates(
        const PF_Point& screen_point,
        const PF_Rect& bounds);

    PF_Point GetScreenCoordinates(
        const CurvePoint& curve_point,
        const PF_Rect& bounds);
};