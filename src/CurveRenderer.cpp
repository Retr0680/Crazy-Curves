#include "CurveRenderer.hpp"
#include <algorithm>

CurveRenderer::CurveRenderer() {}

void CurveRenderer::render(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings) {
    if (settings.showGrid) {
        drawGrid(buffer, settings);
    }
    drawCurve(buffer, curve, settings);
    drawControlPoints(buffer, curve, settings);
}

void CurveRenderer::drawGrid(uint8_t* buffer, const RenderSettings& settings) {
    const int GRID_DIVISIONS = 10;
    
    for (int i = 0; i <= GRID_DIVISIONS; ++i) {
        float t = static_cast<float>(i) / GRID_DIVISIONS;
        int x = static_cast<int>(t * settings.width);
        int y = static_cast<int>(t * settings.height);
        
        // Draw vertical line
        DrawingUtils::drawLine(buffer, settings.width, settings.height, settings.stride,
                             x, 0, x, settings.height, settings.gridColor);
        
        // Draw horizontal line
        DrawingUtils::drawLine(buffer, settings.width, settings.height, settings.stride,
                             0, y, settings.width, y, settings.gridColor);
    }
}

void CurveRenderer::drawCurve(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings) {
    const int SEGMENTS = settings.width;
    float lastY = curve.evaluate(0.0f);
    
    for (int i = 1; i <= SEGMENTS; ++i) {
        float t = static_cast<float>(i) / SEGMENTS;
        float y = curve.evaluate(t);
        
        int x1 = static_cast<int>((i - 1) * settings.width / SEGMENTS);
        int y1 = static_cast<int>((1.0f - lastY) * settings.height);
        int x2 = static_cast<int>(i * settings.width / SEGMENTS);
        int y2 = static_cast<int>((1.0f - y) * settings.height);
        
        DrawingUtils::drawLine(buffer, settings.width, settings.height, settings.stride,
                             x1, y1, x2, y2, settings.lineColor);
        
        lastY = y;
    }
}

void CurveRenderer::drawControlPoints(uint8_t* buffer, const CurveData& curve, const RenderSettings& settings) {
    const int POINT_RADIUS = 4;
    const Color pointColor(255, 255, 255, 255);
    
    for (const auto& point : curve.getPoints()) {
        int x = static_cast<int>(point.x * settings.width);
        int y = static_cast<int>((1.0f - point.y) * settings.height);
        
        DrawingUtils::drawCircle(buffer, settings.width, settings.height, settings.stride,
                               x, y, POINT_RADIUS, pointColor, true);
    }
}