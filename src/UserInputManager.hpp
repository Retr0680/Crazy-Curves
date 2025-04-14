#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <map>

class UserInputManager {
public:
    static UserInputManager& getInstance() {
        static UserInputManager instance;
        return instance;
    }

    PF_Err HandleMouseEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        const PF_EventExtra* event);

    PF_Err HandleKeyboardEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        const PF_EventExtra* event);

    PF_Err HandleDragEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        const PF_EventExtra* event);

private:
    struct InputState {
        PF_Point lastMousePos;
        bool isDragging;
        A_long selectedPoint;
        A_long activeChannel;
        bool modifierKeyPressed;
    };

    InputState currentState;

    PF_Err UpdateCurvePoint(
        PF_InData* in_data,
        const PF_Point& point);

    PF_Err AddCurvePoint(
        PF_InData* in_data,
        const PF_Point& point);

    PF_Err DeleteSelectedPoint(PF_InData* in_data);
};