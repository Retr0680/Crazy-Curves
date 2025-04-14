#include "UserInputManager.hpp"
#include "ViewportManager.hpp"
#include "EventCommunicator.hpp"

PF_Err UserInputManager::HandleMouseEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    const PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    switch (event->e_type) {
        case PF_Event_MOUSE_DOWN:
            currentState.isDragging = true;
            currentState.lastMousePos = event->u.do_click.screen_point;

            // Find nearest point or add new point
            PF_Point worldPoint = ViewportManager::getInstance().ScreenToWorld(
                event->u.do_click.screen_point);

            if (event->u.do_click.modifier_flags & PF_Mod_COMMAND) {
                ERR(AddCurvePoint(in_data, worldPoint));
            } else {
                ERR(UpdateCurvePoint(in_data, worldPoint));
            }
            break;

        case PF_Event_MOUSE_UP:
            currentState.isDragging = false;
            currentState.selectedPoint = -1;
            break;
    }

    if (!err) {
        out_data->out_flags |= PF_OutFlag_REFRESH_UI;
        EventCommunicator::getInstance().BroadcastEvent(
            in_data,
            EventType::CurveModified,
            nullptr);
    }

    return err;
}

PF_Err UserInputManager::HandleKeyboardEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    const PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;

    switch (event->u.do_key.key_code) {
        case PF_KeyCode_DELETE:
        case PF_KeyCode_BACK:
            if (currentState.selectedPoint >= 0) {
                ERR(DeleteSelectedPoint(in_data));
            }
            break;

        case PF_KeyCode_TAB:
            // Cycle through channels
            currentState.activeChannel = (currentState.activeChannel + 1) % 4;
            out_data->out_flags |= PF_OutFlag_REFRESH_UI;
            break;
    }

    currentState.modifierKeyPressed = 
        (event->u.do_key.modifier_flags & (PF_Mod_SHIFT | PF_Mod_COMMAND)) != 0;

    return err;
}

PF_Err UserInputManager::HandleDragEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    const PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;

    if (currentState.isDragging && currentState.selectedPoint >= 0) {
        PF_Point delta;
        delta.h = event->u.do_drag.screen_point.h - currentState.lastMousePos.h;
        delta.v = event->u.do_drag.screen_point.v - currentState.lastMousePos.v;

        // Update point position with constraints if modifier key is pressed
        PF_Point worldPoint = ViewportManager::getInstance().ScreenToWorld(
            event->u.do_drag.screen_point);

        if (currentState.modifierKeyPressed) {
            // Constrain to horizontal/vertical movement
            if (abs(delta.h) > abs(delta.v)) {
                worldPoint.v = currentState.lastMousePos.v;
            } else {
                worldPoint.h = currentState.lastMousePos.h;
            }
        }

        ERR(UpdateCurvePoint(in_data, worldPoint));
        currentState.lastMousePos = event->u.do_drag.screen_point;

        out_data->out_flags |= PF_OutFlag_REFRESH_UI;
        EventCommunicator::getInstance().BroadcastEvent(
            in_data,
            EventType::PointMoved,
            nullptr);
    }

    return err;
}

PF_Err UserInputManager::UpdateCurvePoint(
    PF_InData* in_data,
    const PF_Point& point)
{
    PF_Err err = PF_Err_NONE;

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    CurveData* curve = nullptr;
    switch (currentState.activeChannel) {
        case 0: curve = &seqData->rgb_curve; break;
        case 1: curve = &seqData->r_curve; break;
        case 2: curve = &seqData->g_curve; break;
        case 3: curve = &seqData->b_curve; break;
    }

    if (curve && currentState.selectedPoint >= 0 && 
        currentState.selectedPoint < curve->num_points) {
        curve->points[currentState.selectedPoint].x = 
            std::max(0.0, std::min(1.0, static_cast<double>(point.h)));
        curve->points[currentState.selectedPoint].y = 
            std::max(0.0, std::min(1.0, static_cast<double>(point.v)));
        curve->dirty = true;
    }

    return err;
}

PF_Err UserInputManager::AddCurvePoint(
    PF_InData* in_data,
    const PF_Point& point)
{
    PF_Err err = PF_Err_NONE;

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    CurveData* curve = nullptr;
    switch (currentState.activeChannel) {
        case 0: curve = &seqData->rgb_curve; break;
        case 1: curve = &seqData->r_curve; break;
        case 2: curve = &seqData->g_curve; break;
        case 3: curve = &seqData->b_curve; break;
    }

    if (curve && curve->num_points < 256) {
        // Insert point at correct position
        A_long insertIndex = 0;
        while (insertIndex < curve->num_points && 
               curve->points[insertIndex].x < point.h) {
            insertIndex++;
        }

        // Shift existing points
        for (A_long i = curve->num_points; i > insertIndex; i--) {
            curve->points[i] = curve->points[i - 1];
        }

        // Add new point
        curve->points[insertIndex].x = 
            std::max(0.0, std::min(1.0, static_cast<double>(point.h)));
        curve->points[insertIndex].y = 
            std::max(0.0, std::min(1.0, static_cast<double>(point.v)));
        curve->num_points++;
        curve->dirty = true;

        currentState.selectedPoint = insertIndex;
        
        EventCommunicator::getInstance().BroadcastEvent(
            in_data,
            EventType::PointAdded,
            nullptr);
    }

    return err;
}

PF_Err UserInputManager::DeleteSelectedPoint(PF_InData* in_data)
{
    PF_Err err = PF_Err_NONE;

    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    CurveData* curve = nullptr;
    switch (currentState.activeChannel) {
        case 0: curve = &seqData->rgb_curve; break;
        case 1: curve = &seqData->r_curve; break;
        case 2: curve = &seqData->g_curve; break;
        case 3: curve = &seqData->b_curve; break;
    }

    if (curve && currentState.selectedPoint >= 0 && 
        currentState.selectedPoint < curve->num_points &&
        curve->num_points > 2) {  // Keep at least 2 points
        
        // Shift remaining points
        for (A_long i = currentState.selectedPoint; i < curve->num_points - 1; i++) {
            curve->points[i] = curve->points[i + 1];
        }
        
        curve->num_points--;
        curve->dirty = true;
        currentState.selectedPoint = -1;

        EventCommunicator::getInstance().BroadcastEvent(
            in_data,
            EventType::PointRemoved,
            nullptr);
    }

    return err;
}