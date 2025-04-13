#pragma once

#include "AEConfig.h"
#include "AE_Effect.h"
#include "AE_EffectUI.h"
#include "AE_EffectCB.h"
#include "CurvesData.h"

class CurvesUI {
public:
    CurvesUI();
    
    PF_Err drawCurve(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], 
                     const CurveData& curveData, PF_EffectWorld* world);
                     
    PF_Err handleEvent(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[],
                       PF_LayerDef* output, PF_EventExtra* event_extra);
                       
private:
    void drawGrid(PF_EffectWorld* world);
    void drawCurvePoints(PF_EffectWorld* world, const CurveData& curveData);
    void drawCurveLine(PF_EffectWorld* world, const CurveData& curveData);
    
    // UI Constants
    static const int GRID_DIVISIONS = 8;
    static const int POINT_SIZE = 6;
    static const PF_Pixel GRID_COLOR;
    static const PF_Pixel CURVE_COLOR;
    static const PF_Pixel POINT_COLOR;
};