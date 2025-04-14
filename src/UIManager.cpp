#include "UIManager.hpp"
#include "DrawingUtils.hpp"
#include "ChannelManager.hpp"
#include "PresetManager.hpp"

PF_Err UIManager::DrawUI(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // Draw curve editor
    PF_Rect editorBounds = {
        10, 10,
        10 + 256, // Standard curve editor width
        10 + 256  // Standard curve editor height
    };
    
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;
    
    ERR(DrawCurveEditor(in_data, out_data->effect_win, editorBounds, &seqData->rgb_curve));
    
    // Draw channel selectors
    PF_Point channelOrigin = {
        static_cast<A_short>(editorBounds.right + 20),
        static_cast<A_short>(editorBounds.top)
    };
    ERR(DrawChannelSelectors(in_data, out_data->effect_win, channelOrigin));
    
    // Draw additional controls
    PF_Point controlsOrigin = {
        static_cast<A_short>(editorBounds.right + 20),
        static_cast<A_short>(channelOrigin.v + 120)
    };
    ERR(DrawControls(in_data, out_data->effect_win, controlsOrigin));
    
    return err;
}

PF_Err UIManager::HandleUIEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    switch (event->e_type) {
        case PF_Event_MOUSE_DOWN:
            ERR(HandleMouseDown(in_data, out_data, seqData, event->u.do_click.screen_point));
            break;
            
        case PF_Event_DRAG:
            ERR(HandleDrag(in_data, out_data, seqData, event->u.drag_event.screen_point));
            break;
            
        case PF_Event_KEYDOWN:
            ERR(HandleKeyDown(in_data, out_data, seqData, event->u.key_event));
            break;
    }
    
    return err;
}

PF_Err UIManager::DrawCurveEditor(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Draw grid
    PF_Pixel8 gridColor = {32, 32, 32, 255};
    for (int i = 0; i <= 8; i++) {
        A_long x = bounds.left + (i * (bounds.right - bounds.left) / 8);
        A_long y = bounds.top + (i * (bounds.bottom - bounds.top) / 8);
        
        // Draw vertical and horizontal lines
        for (A_long py = bounds.top; py < bounds.bottom; py++) {
            ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, x, py, &gridColor));
        }
        for (A_long px = bounds.left; px < bounds.right; px++) {
            ERR(suites.PF_WorldSuite1()->PF_SetPixel8(world, px, y, &gridColor));
        }
    }

    // Draw curve
    PF_Pixel8 curveColor = {255, 255, 255, 255};
    for (A_long i = 0; i < curve->num_points - 1; i++) {
        PF_Point p1 = GetScreenCoordinates(curve->points[i], bounds);
        PF_Point p2 = GetScreenCoordinates(curve->points[i + 1], bounds);
        
        // Draw line segment
        DrawingUtils::drawLine(
            reinterpret_cast<uint8_t*>(world->data),
            world->width,
            world->height,
            world->rowbytes,
            p1.h, p1.v,
            p2.h, p2.v,
            {curveColor.red, curveColor.green, curveColor.blue, curveColor.alpha});
    }

    return err;
}

// Continue implementing other methods...