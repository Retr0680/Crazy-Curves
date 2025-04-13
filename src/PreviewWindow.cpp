#include "PreviewWindow.hpp"
#include "DrawingUtils.hpp"

PreviewWindow::PreviewWindow() 
    : isDirty(true)
    , previewBuffer(nullptr)
{
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = PREVIEW_WIDTH;
    bounds.bottom = PREVIEW_HEIGHT;
}

PreviewWindow::~PreviewWindow() {
    if (previewBuffer) {
        PF_DISPOSE_WORLDPTR(previewBuffer);
    }
}

PF_Err PreviewWindow::draw(PF_EffectWorld* world, const PF_Point& origin, 
                          PF_LayerDef* input, PF_LayerDef* output) 
{
    PF_Err err = PF_Err_NONE;
    
    // Update preview bounds based on origin
    bounds.left = origin.h;
    bounds.top = origin.v;
    bounds.right = bounds.left + PREVIEW_WIDTH;
    bounds.bottom = bounds.top + PREVIEW_HEIGHT;

    // Draw border
    ERR(drawBorder(world, bounds));

    // Draw preview content if needed
    if (isDirty) {
        ERR(drawPreviewContent(in_data, world, input, output));
        isDirty = false;
    }

    return err;
}

PF_Err PreviewWindow::drawBorder(PF_EffectWorld* world, const PF_Rect& rect) {
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Draw rectangle border
    PF_Pixel borderColor = {128, 128, 128, 255};
    
    for (A_long i = rect.left; i < rect.right; i++) {
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, i, rect.top, &borderColor));
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, i, rect.bottom-1, &borderColor));
    }
    
    for (A_long i = rect.top; i < rect.bottom; i++) {
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, rect.left, i, &borderColor));
        ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, rect.right-1, i, &borderColor));
    }

    return err;
}

PF_Err PreviewWindow::drawPreviewContent(PF_InData* in_data, PF_EffectWorld* world, PF_LayerDef* input, PF_LayerDef* output) {
    PF_Err err = PF_Err_NONE;
    // Scale down input for preview
    downscaleInput(input);
    
    // Apply current curve to preview
    // This would use the current curve settings to process the downscaled image

    return err;
}

void PreviewWindow::downscaleInput(PF_LayerDef* input) {
    // Implement downscaling of input image to preview size
    // This would use bilinear interpolation to maintain image quality
}

void PreviewWindow::update(const CurveData& currentCurve) {
    isDirty = true;
}

PF_Boolean PreviewWindow::isMouseOver(const PF_Point& point) const {
    return (point.h >= bounds.left && point.h < bounds.right &&
            point.v >= bounds.top && point.v < bounds.bottom);
}