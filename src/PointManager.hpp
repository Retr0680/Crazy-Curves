#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class PointManager {
public:
    static PF_Err FindNearestPoint(
        const CurveData* curve,
        const PF_Point& point,
        const PF_Rect& bounds,
        A_long* selectedIndex,
        PF_FpLong threshold = 5.0);

    static PF_Err ValidatePointMove(
        const CurveData* curve,
        A_long pointIndex,
        PF_FpLong newX,
        PF_FpLong newY,
        bool* isValid);

    static PF_Point GetScreenCoordinates(
        const CurvePoint& point,
        const PF_Rect& bounds);

    static CurvePoint GetCurveCoordinates(
        const PF_Point& screen_point,
        const PF_Rect& bounds);

private:
    static PF_FpLong CalculateDistance(
        const PF_Point& p1,
        const PF_Point& p2);
};