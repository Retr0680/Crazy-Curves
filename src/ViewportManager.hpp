#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ViewportManager {
public:
    static ViewportManager& getInstance() {
        static ViewportManager instance;
        return instance;
    }

    PF_Err SetupViewport(
        PF_InData* in_data,
        const PF_Rect& rect);

    PF_Err UpdateViewport(
        PF_InData* in_data,
        PF_OutData* out_data,
        const CurveData* curves);

    PF_Point WorldToScreen(const CurvePoint& point) const;
    CurvePoint ScreenToWorld(const PF_Point& point) const;

    PF_Err Pan(PF_FpLong dx, PF_FpLong dy);
    PF_Err Zoom(PF_FpLong factor, const PF_Point& center);
    PF_Err ResetView();

private:
    ViewportManager() : 
        zoomLevel(1.0),
        panX(0.0),
        panY(0.0),
        viewportRect{0,0,0,0} {}

    PF_FpLong zoomLevel;
    PF_FpLong panX, panY;
    PF_Rect viewportRect;

    struct ViewportTransform {
        PF_FpLong scaleX, scaleY;
        PF_FpLong offsetX, offsetY;
    };

    ViewportTransform ComputeTransform() const;
    bool IsPointVisible(const PF_Point& point) const;
};