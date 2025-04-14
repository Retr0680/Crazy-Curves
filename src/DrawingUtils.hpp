#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <string>

class DrawingUtils {
public:
    static PF_Err DrawRect(
        PF_EffectWorld* world,
        const PF_Rect& rect,
        const PF_Pixel8& color);

    static PF_Err DrawLine(
        PF_EffectWorld* world,
        A_long x1, A_long y1,
        A_long x2, A_long y2,
        const PF_Pixel8& color);

    static PF_Err DrawText(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const std::string& text,
        const PF_Point& position,
        const PF_Pixel8& color);

    static PF_Err DrawPoint(
        PF_EffectWorld* world,
        const PF_Point& center,
        A_long radius,
        const PF_Pixel8& color,
        bool filled = true);

    static bool PointInRect(
        const PF_Point& point,
        const PF_Rect& rect);

private:
    static void PlotPixel(
        PF_EffectWorld* world,
        A_long x, A_long y,
        const PF_Pixel8& color);
};