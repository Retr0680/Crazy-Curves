#pragma once
#include <memory>
#include "CurveData.hpp"
#include "DrawingUtils.hpp"

class CurveRenderer {
public:
    struct RenderSettings {
        int width;
        int height;
        int stride;
        float lineThickness;
        Color lineColor;
        Color gridColor;
        bool showGrid;
    };

    CurveRenderer();
    void render(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings);

private:
    void drawGrid(uint8_t* buffer, const RenderSettings& settings);
    void drawCurve(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings);
    void drawControlPoints(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings);
};