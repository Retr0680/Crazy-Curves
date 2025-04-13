#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CurveUIControl {
public:
    static PF_Err Draw(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds,
        const CurveData* curve);

    static PF_Err HandleClick(
        PF_InData* in_data,
        PF_OutData* out_data,
        const PF_Point& point,
        CurveData* curve);

private:
    static const A_long GRID_DIVISIONS = 8;
    static const A_long POINT_SIZE = 4;
    
    static PF_Err DrawGrid(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const PF_Rect& bounds);
        
    static PF_Err DrawCurve(
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