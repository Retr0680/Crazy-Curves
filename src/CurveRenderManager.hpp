#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class CurveRenderManager {
public:
    static CurveRenderManager& getInstance() {
        static CurveRenderManager instance;
        return instance;
    }

    PF_Err RenderCurve(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve,
        const PF_Pixel8& color);

    PF_Err RenderControlPoints(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve,
        const PF_Pixel8& color,
        A_long selectedPoint = -1);

    PF_Err RenderGrid(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const PF_Pixel8& color);

private:
    static const A_long POINT_RADIUS = 4;
    static const A_long SELECTED_POINT_RADIUS = 6;
    static const A_long GRID_DIVISIONS = 8;

    PF_Err DrawLine(
        PF_EffectWorld* world,
        A_long x1, A_long y1,
        A_long x2, A_long y2,
        const PF_Pixel8& color);

    PF_Err DrawPoint(
        PF_EffectWorld* world,
        A_long x, A_long y,
        A_long radius,
        const PF_Pixel8& color,
        bool filled = true);

    PF_Point WorldToScreen(
        const CurvePoint& point,
        const PF_Rect& bounds);
};