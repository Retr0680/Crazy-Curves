#include "MouseHandler.hpp"
#include <cmath>

MouseHandler::MouseHandler() 
    : dragState(DragState::None)
    , activeCurve(nullptr)
    , selectedPointIndex(0) {
}

bool MouseHandler::handleMouseDown(const PF_Point& point, bool isShiftDown) {
    if (!activeCurve) return false;
    
    lastMousePos = point;
    const auto& points = activeCurve->getPoints();
    
    // Check for hits on control points first
    for (size_t i = 0; i < points.size(); ++i) {
        const auto& p = points[i];
        if (hitTest(point, p.x, p.y)) {
            selectedPointIndex = i;
            dragState = DragState::DraggingPoint;
            return true;
        }
    }
    
    // If shift is down, add a new point
    if (isShiftDown) {
        float x = snapToGrid(point.x, true);
        float y = snapToGrid(point.y, true);
        activeCurve->addPoint(x, y);
        return true;
    }
    
    return false;
}

bool MouseHandler::handleMouseDrag(const PF_Point& point) {
    if (!activeCurve || dragState == DragState::None) return false;
    
    float dx = point.x - lastMousePos.x;
    float dy = point.y - lastMousePos.y;
    
    switch (dragState) {
        case DragState::DraggingPoint: {
            const auto& points = activeCurve->getPoints();
            if (selectedPointIndex < points.size()) {
                float newX = snapToGrid(points[selectedPointIndex].x + dx, false);
                float newY = snapToGrid(points[selectedPointIndex].y + dy, false);
                activeCurve->updatePoint(selectedPointIndex, newX, newY);
            }
            break;
        }
        case DragState::DraggingControlPoint1:
        case DragState::DraggingControlPoint2:
            // Handle control point dragging for Bezier curves
            break;
        default:
            break;
    }
    
    lastMousePos = point;
    return true;
}

bool MouseHandler::handleMouseUp(const PF_Point& point) {
    dragState = DragState::None;
    return true;
}

void MouseHandler::setActiveCurve(CurveData* curve) {
    activeCurve = curve;
}

float MouseHandler::snapToGrid(float value, bool isShiftDown) {
    const float gridSize = isShiftDown ? 0.1f : 0.05f;
    return std::round(value / gridSize) * gridSize;
}

bool MouseHandler::hitTest(const PF_Point& point, float x, float y, float threshold) {
    float dx = point.x - x;
    float dy = point.y - y;
    return (dx * dx + dy * dy) <= (threshold * threshold);
}