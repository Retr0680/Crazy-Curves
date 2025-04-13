#pragma once
#include "AE_Effect.h"
#include "AE_EffectUI.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class PreviewWindow {
public:
    static const A_long PREVIEW_WIDTH = 200;
    static const A_long PREVIEW_HEIGHT = 150;
    static const A_long BORDER_SIZE = 2;
    
    PreviewWindow();
    ~PreviewWindow();

    PF_Err draw(PF_EffectWorld* world, const PF_Point& origin, 
                PF_LayerDef* input, PF_LayerDef* output);
    PF_Err update(const CurveData* curve);
    PF_Boolean isMouseOver(const PF_Point& point) const;
    void invalidate() { isDirty = true; }

private:
    PF_Rect bounds;
    PF_Boolean isDirty;
    PF_EffectWorld* previewBuffer;
    
    PF_Err drawBorder(PF_EffectWorld* world, const PF_Rect& rect);
    PF_Err drawPreviewContent(PF_InData* in_data, PF_EffectWorld* world, 
                            PF_LayerDef* input, PF_LayerDef* output);
    PF_Err downscaleInput(PF_InData* in_data, PF_LayerDef* input);
};