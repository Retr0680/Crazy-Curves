#pragma once
#include "AE_Effect.h"

namespace DrawingUtils {
    void drawRect(PF_EffectWorld* world, const PF_Rect& rect, const PF_Pixel& color);
    void drawLine(PF_EffectWorld* world, int x1, int y1, int x2, int y2, const PF_Pixel& color);
    void drawText(PF_EffectWorld* world, const char* text, const PF_Rect& bounds, const PF_Pixel& color);
    bool pointInRect(const PF_Point& point, const PF_Rect& rect);
}