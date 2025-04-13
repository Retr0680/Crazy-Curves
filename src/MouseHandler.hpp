#pragma once
#include "AE_Effect.h"
#include "CurveData.hpp"

enum class DragState {
    None,
    DraggingPoint,
    DraggingControlPoint1,
    DraggingControlPoint2
};

class MouseHandler {
public:
    MouseHandler();
    
    bool handleMouseDown(const PF_Point& point, bool isShiftDown);
    bool handleMouseDrag(const PF_Point& point);
    bool handleMouseUp(const PF_Point& point);
    
    void setActiveCurve(CurveData* curve);
    bool isDragging() const { return dragState != DragState::None; }
    
private:
    DragState dragState;
    CurveData* activeCurve;
    size_t selectedPointIndex;
    PF_Point lastMousePos;
    
    float snapToGrid(float value, bool isShiftDown);
    bool hitTest(const PF_Point& point, float x, float y, float threshold = 5.0f);
};