#pragma once
#include "AE_Effect.h"
#include "CurveData.hpp"

class PreviewWindow {
public:
    static const int PREVIEW_WIDTH = 200;
    static const int PREVIEW_HEIGHT = 150;
    static const int BORDER_SIZE = 2;
    
    PreviewWindow();
    void draw(PF_EffectWorld* world, const PF_Point& origin, 
             PF_LayerDef* input, PF_LayerDef* output);
    void update(const CurveData& currentCurve);
    bool isMouseOver(const PF_Point& point) const;

private:
    PF_Rect bounds;
    bool isDirty;
    PF_EffectWorld previewBuffer;
    
    void drawBorder(PF_EffectWorld* world, const PF_Rect& rect);
    void drawPreviewContent(PF_EffectWorld* world, PF_LayerDef* input, PF_LayerDef* output);
    void downscaleInput(PF_LayerDef* input);
};