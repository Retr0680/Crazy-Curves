#include "PreviewWindow.hpp"
#include "DrawingUtils.hpp"

PreviewWindow::PreviewWindow() : isDirty(true) {
    bounds = {
        .left = 0,
        .top = 0,
        .right = PREVIEW_WIDTH,
        .bottom = PREVIEW_HEIGHT
    };
}

void PreviewWindow::draw(PF_EffectWorld* world, const PF_Point& origin, 
                        PF_LayerDef* input, PF_LayerDef* output) {
    // Draw preview frame
    PF_Rect frameBounds = DrawingUtils::offsetRect(bounds, origin);
    DrawingUtils::drawRect(world, frameBounds, PF_Pixel{200, 200, 200, 255});

    // Draw preview content
    if (isDirty) {
        // Update preview content here
        isDirty = false;
    }

    // Draw preview title
    PF_Rect titleBounds = {
        frameBounds.left + 5,
        frameBounds.top + 5,
        frameBounds.right - 5,
        frameBounds.top + 20
    };
    DrawingUtils::drawText(world, "Preview", titleBounds, PF_Pixel{255, 255, 255, 255});
}

void PreviewWindow::update(const CurveData& currentCurve) {
    isDirty = true;
}