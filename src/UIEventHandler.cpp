#include "UIEventHandler.hpp"

PF_Err UIEventHandler::HandleEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    switch (event->e_type) {
        case PF_Event_DO_CLICK:
            err = HandleMouseDown(in_data, out_data, seqData, event->u.do_click.screen_point);
            break;

        case PF_Event_DRAG:
            err = HandleDrag(in_data, out_data, seqData, event->u.drag_event.screen_point);
            break;

        case PF_Event_KEYDOWN:
            err = HandleKeyDown(in_data, out_data, seqData, event->u.key_event);
            break;
    }

    return err;
}

PF_Err UIEventHandler::HandleMouseDown(
    PF_InData* in_data,
    PF_OutData* out_data,
    SequenceData* seqData,
    const PF_Point& point)
{
    PF_Point curve_point = GetCurveSpaceCoordinates(point);
    
    if (curve_point.h >= 0 && curve_point.h <= 1.0 && 
        curve_point.v >= 0 && curve_point.v <= 1.0) {
        
        CurvePoint newPoint = {curve_point.h, curve_point.v};
        CurveData* activeCurve = &seqData->rgb_curve;
        
        if (activeCurve->num_points < 256) {
            activeCurve->points[activeCurve->num_points++] = newPoint;
            activeCurve->dirty = true;
            out_data->refresh_flag = true;
        }
    }
    
    return PF_Err_NONE;
}

PF_Err UIEventHandler::HandleDrag(
    PF_InData* in_data,
    PF_OutData* out_data,
    SequenceData* seqData,
    const PF_Point& point)
{
    if (seqData->selected_point >= 0 && 
        seqData->selected_point < seqData->rgb_curve.num_points) {
        
        PF_Point curve_point = GetCurveSpaceCoordinates(point);
        
        // Clamp values between 0 and 1
        PF_FpLong x = PF_MAX(0, PF_MIN(1, curve_point.h));
        PF_FpLong y = PF_MAX(0, PF_MIN(1, curve_point.v));
        
        seqData->rgb_curve.points[seqData->selected_point].x = x;
        seqData->rgb_curve.points[seqData->selected_point].y = y;
        seqData->rgb_curve.dirty = true;
        out_data->refresh_flag = true;
    }
    
    return PF_Err_NONE;
}

PF_Err UIEventHandler::HandleKeyDown(
    PF_InData* in_data,
    PF_OutData* out_data,
    SequenceData* seqData,
    const PF_KeyDownEvent& key_event)
{
    if (key_event.key_code == PF_KeyCode_DELETE) {
        if (seqData->selected_point >= 0 && 
            seqData->selected_point < seqData->rgb_curve.num_points) {
            
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
    
    return PF_Err_NONE;
}

PF_Point UIEventHandler::GetCurveSpaceCoordinates(const PF_Point& screen_point)
{
    PF_Point curve_point;
    curve_point.h = (screen_point.h - 10) / 256.0;
    curve_point.v = 1.0 - ((screen_point.v - 10) / 256.0);
    return curve_point;
}