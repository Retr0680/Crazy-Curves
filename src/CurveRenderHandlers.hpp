#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CurveRenderHandlers {
public:
    static PF_Err RenderCurve(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve,
        const PF_Pixel8& color);

    static PF_Err RenderPoints(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve,
        const PF_Pixel8& color,
        A_long selectedPoint);

private:
    static const A_long POINT_SIZE = 4;
    static const A_long SELECTED_POINT_SIZE = 6;
};