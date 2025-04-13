#pragma once

#include "AE_Effect.h"
#include "ChannelManager.hpp"
#include "PreviewWindow.hpp"
#include "CurveHistory.hpp"

class CurvesUI {
public:
    CurvesUI();
    
    PF_Err drawCurve(PF_InData* in_data, PF_OutData* out_data, 
                     PF_ParamDef* params[], const CurveData& curveData, 
                     PF_EffectWorld* world);
                     
    PF_Err handleEvent(PF_InData* in_data, PF_OutData* out_data, 
                      PF_ParamDef* params[], PF_LayerDef* output, 
                      PF_EventExtra* event);

private:
    ChannelManager channelManager;
    PreviewWindow preview;
    CurveHistory history;
    
    void drawGrid(PF_EffectWorld* world);
    void drawCurvePoints(PF_EffectWorld* world, const CurveData& curveData);
    void drawCurveLine(PF_EffectWorld* world, const CurveData& curveData);
};