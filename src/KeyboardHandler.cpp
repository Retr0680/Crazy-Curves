#include "KeyboardHandler.hpp"

PF_Err KeyboardHandler::HandleKeyEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    SequenceData* seqData,
    const PF_KeyDownEvent* key_event)
{
    PF_Err err = PF_Err_NONE;

    switch (key_event->key_code) {
        case PF_KeyCode_DELETE:
        case PF_KeyCode_BACKSPACE:
            err = HandleDelete(in_data, out_data, seqData);
            break;

        default:
            err = HandleModifiers(in_data, out_data, seqData, key_event);
            break;
    }

    return err;
}

PF_Err KeyboardHandler::HandleDelete(
    PF_InData* in_data,
    PF_OutData* out_data, 
    SequenceData* seqData)
{
    PF_Err err = PF_Err_NONE;

    if (seqData->selected_point >= 0 && 
        seqData->selected_point < seqData->rgb_curve.num_points) {
        
        // Don't allow deleting first or last point
        if (seqData->selected_point > 0 && 
            seqData->selected_point < seqData->rgb_curve.num_points - 1) {

            // Remove selected point
            for (A_long i = seqData->selected_point; 
                 i < seqData->rgb_curve.num_points - 1; i++) {
                seqData->rgb_curve.points[i] = seqData->rgb_curve.points[i + 1];
            }
            seqData->rgb_curve.num_points--;
            seqData->rgb_curve.dirty = true;
            seqData->selected_point = -1;
            out_data->refresh_flag = true;
        }
    }

    return err;
}

PF_Err KeyboardHandler::HandleModifiers(
    PF_InData* in_data,
    PF_OutData* out_data,
    SequenceData* seqData,
    const PF_KeyDownEvent* key_event)
{
    PF_Err err = PF_Err_NONE;
    
    // Handle shift for constrained movement
    if (key_event->modifier_flags & PF_Mod_SHIFT_KEY) {
        if (seqData->selected_point >= 0) {
            CurvePoint* point = &seqData->rgb_curve.points[seqData->selected_point];
            // Snap to 45 degree angles
            PF_FpLong dx = point->x - 0.5f;
            PF_FpLong dy = point->y - 0.5f;
            if (fabs(dx) > fabs(dy)) {
                point->y = 0.5f;
            } else {
                point->x = 0.5f;
            }
            seqData->rgb_curve.dirty = true;
            out_data->refresh_flag = true;
        }
    }

    return err;
}