#pragma once
#include "AE_Effect.h"
#include "CurveData.hpp"

class PreviewWindow {
public:
    static const int PREVIEW_WIDTH = 200;
    static const int PREVIEW_HEIGHT = 150;
    
    PreviewWindow();
    void draw(PF_EffectWorld* world, const PF_Point& origin, 
             PF_LayerDef* input, PF_LayerDef* output);
    void update(const CurveData& currentCurve);

private:
    PF_Rect bounds;
    bool isDirty;
    PF_EffectWorld previewBuffer;
};