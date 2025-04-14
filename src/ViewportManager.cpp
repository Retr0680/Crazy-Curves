#include "ViewportManager.hpp"
#include <algorithm>

PF_Err ViewportManager::SetupViewport(
    PF_InData* in_data,
    const PF_Rect& rect)
{
    PF_Err err = PF_Err_NONE;

    viewportRect = rect;
    zoomLevel = 1.0;
    panX = panY = 0.0;

    return err;
}

PF_Err ViewportManager::UpdateViewport(
    PF_InData* in_data,
    PF_OutData* out_data,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    // Request viewport redraw
    out_data->out_flags |= PF_OutFlag_REFRESH_UI;

    // Calculate optimal view bounds based on curve points
    if (curves && curves->num_points > 0) {
        PF_FpLong minX = 1.0, minY = 1.0;
        PF_FpLong maxX = 0.0, maxY = 0.0;

        // Find curve bounds
        for (A_long i = 0; i < curves->num_points; i++) {
            minX = std::min(minX, curves->points[i].x);
            minY = std::min(minY, curves->points[i].y);
            maxX = std::max(maxX, curves->points[i].x);
            maxY = std::max(maxY, curves->points[i].y);
        }

        // Add padding
        const PF_FpLong padding = 0.1;
        minX -= padding;
        minY -= padding;
        maxX += padding;
        maxY += padding;

        // Adjust viewport transform
        ViewportTransform transform = ComputeTransform();
        PF_FpLong width = maxX - minX;
        PF_FpLong height = maxY - minY;

        if (width > 0 && height > 0) {
            PF_FpLong scaleX = (viewportRect.right - viewportRect.left) / width;
            PF_FpLong scaleY = (viewportRect.bottom - viewportRect.top) / height;
            zoomLevel = std::min(scaleX, scaleY);
        }
    }

    return err;
}

PF_Point ViewportManager::WorldToScreen(const CurvePoint& point) const {
    ViewportTransform transform = ComputeTransform();
    
    PF_Point screen;
    screen.h = static_cast<A_long>((point.x * transform.scaleX) + transform.offsetX);
    screen.v = static_cast<A_long>((point.y * transform.scaleY) + transform.offsetY);

    return screen;
}

CurvePoint ViewportManager::ScreenToWorld(const PF_Point& point) const {
    ViewportTransform transform = ComputeTransform();
    
    CurvePoint world;
    world.x = (point.h - transform.offsetX) / transform.scaleX;
    world.y = (point.v - transform.offsetY) / transform.scaleY;

    // Clamp to valid range [0,1]
    world.x = std::max(0.0, std::min(1.0, world.x));
    world.y = std::max(0.0, std::min(1.0, world.y));

    return world;
}

PF_Err ViewportManager::Pan(PF_FpLong dx, PF_FpLong dy) {
    PF_Err err = PF_Err_NONE;

    panX += dx / zoomLevel;
    panY += dy / zoomLevel;

    return err;
}

PF_Err ViewportManager::Zoom(PF_FpLong factor, const PF_Point& center) {
    PF_Err err = PF_Err_NONE;

    // Save world position of zoom center
    CurvePoint worldCenter = ScreenToWorld(center);

    // Apply zoom
    const PF_FpLong MIN_ZOOM = 0.1;
    const PF_FpLong MAX_ZOOM = 10.0;
    zoomLevel = std::max(MIN_ZOOM, std::min(MAX_ZOOM, zoomLevel * factor));

    // Adjust pan to keep zoom center fixed
    PF_Point newCenter = WorldToScreen(worldCenter);
    panX += (center.h - newCenter.h) / zoomLevel;
    panY += (center.v - newCenter.v) / zoomLevel;

    return err;
}

PF_Err ViewportManager::ResetView() {
    PF_Err err = PF_Err_NONE;

    zoomLevel = 1.0;
    panX = panY = 0.0;

    return err;
}

ViewportManager::ViewportTransform ViewportManager::ComputeTransform() const {
    ViewportTransform transform;
    
    transform.scaleX = zoomLevel * (viewportRect.right - viewportRect.left);
    transform.scaleY = zoomLevel * (viewportRect.bottom - viewportRect.top);
    transform.offsetX = viewportRect.left + (panX * transform.scaleX);
    transform.offsetY = viewportRect.bottom - (panY * transform.scaleY);

    return transform;
}

bool ViewportManager::IsPointVisible(const PF_Point& point) const {
    return point.h >= viewportRect.left && 
           point.h <= viewportRect.right &&
           point.v >= viewportRect.top && 
           point.v <= viewportRect.bottom;
}