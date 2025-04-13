#include "PreviewWindow.hpp"
#include "DrawingUtils.hpp"

PreviewWindow::PreviewWindow() : isDirty(true) {
    bounds = PF_Rect{
        0,      // left
        0,      // top
        PREVIEW_WIDTH,    // right
        PREVIEW_HEIGHT    // bottom
    };
}

void PreviewWindow::draw(PF_EffectWorld* world, const PF_Point& origin, 
                        PF_LayerDef* input, PF_LayerDef* output) {
    PF_Rect drawBounds = {
        .left = origin.x,
        .top = origin.y,
        .right = origin.x + PREVIEW_WIDTH,
        .bottom = origin.y + PREVIEW_HEIGHT
    };

    // Draw background
    DrawingUtils::drawRect(world, drawBounds, PF_Pixel{32, 32, 32, 255});
    
    // Draw border
    drawBorder(world, drawBounds);
    
    if (isDirty) {
        drawPreviewContent(world, input, output);
        isDirty = false;
    }
}

void PreviewWindow::drawBorder(PF_EffectWorld* world, const PF_Rect& rect) {
    PF_Pixel borderColor = {128, 128, 128, 255};
    
    // Draw border lines
    for (int i = 0; i < BORDER_SIZE; i++) {
        PF_Rect borderRect = {
            .left = rect.left + i,
            .top = rect.top + i,
            .right = rect.right - i,
            .bottom = rect.bottom - i
        };
        DrawingUtils::drawRect(world, borderRect, borderColor);
    }
}

void PreviewWindow::drawPreviewContent(PF_EffectWorld* world, PF_LayerDef* input, PF_LayerDef* output) {
    // Scale down input for preview
    downscaleInput(input);
    
    // Apply current curve to preview
    // This would use the current curve settings to process the downscaled image
}

void PreviewWindow::downscaleInput(PF_LayerDef* input) {
    // Implement downscaling of input image to preview size
    // This would use bilinear interpolation to maintain image quality
}

void PreviewWindow::update(const CurveData& currentCurve) {
    isDirty = true;
}

bool PreviewWindow::isMouseOver(const PF_Point& point) const {
    return point.x >= bounds.left && point.x <= bounds.right &&
           point.y >= bounds.top && point.y <= bounds.bottom;
}