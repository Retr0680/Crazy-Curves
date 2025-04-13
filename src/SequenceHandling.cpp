#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

static PF_Err
ResetCurves(CurveData* curve) {
    PF_Err err = PF_Err_NONE;
    
    curve->curve_id = CURVEDATA_DISK_ID;
    curve->num_points = 2;
    
    // Initialize with linear curve
    curve->points[0].x = 0.0f;
    curve->points[0].y = 0.0f;
    curve->points[1].x = 1.0f;
    curve->points[1].y = 1.0f;
    
    curve->dirty = true;
    
    return err;
}

PF_Err
SequenceResetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = reinterpret_cast<SequenceData*>(out_data->sequence_data);
    
    if (in_data->sequence_data) {
        seqData = reinterpret_cast<SequenceData*>(suites.HandleSuite1()->host_lock_handle(in_data->sequence_data));
        
        if (!seqData) {
            return PF_Err_OUT_OF_MEMORY;
        }
        
        // Reset all curves
        ERR(ResetCurves(&seqData->rgb_curve));
        ERR(ResetCurves(&seqData->r_curve));
        ERR(ResetCurves(&seqData->g_curve));
        ERR(ResetCurves(&seqData->b_curve));
        
        suites.HandleSuite1()->host_unlock_handle(in_data->sequence_data);
    }
    
    return err;
}

PF_Err
UpdateParameterUI(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = reinterpret_cast<SequenceData*>(in_data->sequence_data);
    
    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;
    
    // Update enabled state of curve parameters based on current selection
    params[PARAM_RGB_CURVE]->ui_flags &= ~PF_PUI_DISABLED;
    params[PARAM_R_CURVE]->ui_flags |= PF_PUI_DISABLED;
    params[PARAM_G_CURVE]->ui_flags |= PF_PUI_DISABLED;
    params[PARAM_B_CURVE]->ui_flags |= PF_PUI_DISABLED;
    
    out_data->out_flags |= PF_OutFlag_REFRESH_UI;
    
    return err;
}