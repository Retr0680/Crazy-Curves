#include "CurvesUI.h"

const PF_Pixel CurvesUI::GRID_COLOR = {32, 32, 32, 255};
const PF_Pixel CurvesUI::CURVE_COLOR = {255, 255, 255, 255};
const PF_Pixel CurvesUI::POINT_COLOR = {255, 128, 0, 255};

CurvesUI::CurvesUI() {}

PF_Err CurvesUI::drawCurve(PF_InData* in_data, PF_OutData* out_data, 
                          PF_ParamDef* params[], const CurveData& curveData,
                          PF_EffectWorld* world) {
    PF_Err err = PF_Err_NONE;
    
    // Draw background grid
    drawGrid(world);
    
    // Draw curve line
    drawCurveLine(world, curveData);
    
    // Draw control points
    drawCurvePoints(world, curveData);
    
    return err;
}

void CurvesUI::drawGrid(PF_EffectWorld* world) {
    // Draw grid lines
    for (int i = 0; i <= GRID_DIVISIONS; ++i) {
        float pos = static_cast<float>(i) / GRID_DIVISIONS;
        int x = static_cast<int>(pos * world->width);
        int y = static_cast<int>(pos * world->height);
        
        // Draw vertical line
        for (int row = 0; row < world->height; ++row) {
            PF_Pixel* pixel = (PF_Pixel*)((char*)world->data + 
                                        row * world->rowbytes + 
                                        x * sizeof(PF_Pixel));
            *pixel = GRID_COLOR;
        }
        
        // Draw horizontal line
        PF_Pixel* row = (PF_Pixel*)((char*)world->data + y * world->rowbytes);
        for (int col = 0; col < world->width; ++col) {
            row[col] = GRID_COLOR;
        }
    }
}

void CurvesUI::drawCurveLine(PF_EffectWorld* world, const CurveData& curveData) {
    // Draw curve using small line segments
    float prevX = 0.0f;
    float prevY = curveData.evaluate(0.0f);
    
    for (int i = 1; i <= 100; ++i) {
        float t = static_cast<float>(i) / 100.0f;
        float x = t;
        float y = curveData.evaluate(t);
        
        // Convert to screen coordinates
        int x1 = static_cast<int>(prevX * world->width);
        int y1 = static_cast<int>((1.0f - prevY) * world->height);
        int x2 = static_cast<int>(x * world->width);
        int y2 = static_cast<int>((1.0f - y) * world->height);
        
        // Draw line segment
        drawLine(world, x1, y1, x2, y2, CURVE_COLOR);
        
        prevX = x;
        prevY = y;
    }
}

void CurvesUI::drawCurvePoints(PF_EffectWorld* world, const CurveData& curveData) {
    const auto& points = curveData.getPoints();
    
    for (const auto& point : points) {
        int x = static_cast<int>(point.x * world->width);
        int y = static_cast<int>((1.0f - point.y) * world->height);
        
        // Draw point as a small square
        for (int dy = -POINT_SIZE/2; dy <= POINT_SIZE/2; ++dy) {
            for (int dx = -POINT_SIZE/2; dx <= POINT_SIZE/2; ++dx) {
                int px = x + dx;
                int py = y + dy;
                
                if (px >= 0 && px < world->width && py >= 0 && py < world->height) {
                    PF_Pixel* pixel = (PF_Pixel*)((char*)world->data + 
                                                py * world->rowbytes + 
                                                px * sizeof(PF_Pixel));
                    *pixel = POINT_COLOR;
                }
            }
        }
    }
}

PF_Err CurvesUI::handleEvent(PF_InData* in_data, PF_OutData* out_data,
                            PF_ParamDef* params[], PF_LayerDef* output,
                            PF_EventExtra* event_extra) {
    PF_Err err = PF_Err_NONE;
    
    switch (event_extra->e_type) {
        case PF_Event_MOUSE_DOWN:
            // Handle mouse down event
            break;
            
        case PF_Event_MOUSE_UP:
            // Handle mouse up event
            break;
            
        case PF_Event_MOUSE_DRAG:
            // Handle mouse drag event
            break;
            
        default:
            break;
    }
    
    return err;
}