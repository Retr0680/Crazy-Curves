#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

static PF_Err
UserChangedParam(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    PF_UserChangedParamExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    switch (extra->param_index) {
        case PARAM_RGB_CURVE:
        case PARAM_R_CURVE:
        case PARAM_G_CURVE:
        case PARAM_B_CURVE:
            {
                // Mark cache as dirty when curves change
                seqData->needs_update = true;
                out_data->out_flags |= PF_OutFlag_FORCE_RERENDER;
            }
            break;

        case PARAM_OPACITY:
            {
                // Handle opacity changes
                out_data->out_flags |= PF_OutFlag_REFRESH_UI;
            }
            break;
    }

    return err;
}

static PF_Err
UpdateParameterUI(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Update enabled/disabled state of parameters
    PF_ParamDef* param = params[0];
    
    for (A_long i = PARAM_RGB_CURVE; i <= PARAM_B_CURVE; i++) {
        if (i == PARAM_RGB_CURVE) {
            params[i]->ui_flags &= ~PF_PUI_DISABLED;
        } else {
            // Enable individual channel curves only when RGB curve is active
            if (params[PARAM_RGB_CURVE]->u.bd.value) {
                params[i]->ui_flags &= ~PF_PUI_DISABLED;
            } else {
                params[i]->ui_flags |= PF_PUI_DISABLED;
            }
        }
    }

    out_data->out_flags |= PF_OutFlag_REFRESH_UI;
    return err;
}

static PF_Err
ValidateParameters(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[])
{
    PF_Err err = PF_Err_NONE;
    
    // Validate curve points are within bounds
    for (A_long i = PARAM_RGB_CURVE; i <= PARAM_B_CURVE; i++) {
        CurveData* curve = nullptr;
        switch (i) {
            case PARAM_RGB_CURVE:
                curve = &((SequenceData*)in_data->sequence_data)->rgb_curve;
                break;
            case PARAM_R_CURVE:
                curve = &((SequenceData*)in_data->sequence_data)->r_curve;
                break;
            case PARAM_G_CURVE:
                curve = &((SequenceData*)in_data->sequence_data)->g_curve;
                break;
            case PARAM_B_CURVE:
                curve = &((SequenceData*)in_data->sequence_data)->b_curve;
                break;
        }

        if (curve) {
            for (A_long j = 0; j < curve->num_points; j++) {
                curve->points[j].x = PF_MAX(0, PF_MIN(1, curve->points[j].x));
                curve->points[j].y = PF_MAX(0, PF_MIN(1, curve->points[j].y));
            }
        }
    }
    
    return err;
}