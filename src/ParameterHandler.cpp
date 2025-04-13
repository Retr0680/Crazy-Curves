#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include "ParameterHandler.hpp"

PF_Err ParameterHandler::SetupParams(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[])
{
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    // Add Curves group
    AEFX_CLR_STRUCT(def);
    def.param_type = PF_Param_GROUP_START;
    PF_STRCPY(def.name, "Curves");
    def.flags = PF_ParamFlag_SUPERVISE;
    ERR(PF_ADD_PARAM(in_data, -1, &def));

    // Add RGB Curve
    ERR(AddCurveParam(in_data, "Master", PF_PUI_TOPIC | PF_PUI_CONTROL));

    // Add Individual Channel Curves
    ERR(AddCurveParam(in_data, "Red", PF_PUI_CONTROL));
    ERR(AddCurveParam(in_data, "Green", PF_PUI_CONTROL));
    ERR(AddCurveParam(in_data, "Blue", PF_PUI_CONTROL));

    // Add Opacity slider
    AEFX_CLR_STRUCT(def);
    PF_STRCPY(def.name, "Opacity");
    def.param_type = PF_Param_FLOAT_SLIDER;
    def.u.fs_d.valid_min = 0;
    def.u.fs_d.valid_max = 100;
    def.u.fs_d.value = 100;
    def.u.fs_d.dephault = 100;
    def.flags = PF_ParamFlag_SUPERVISE;
    ERR(PF_ADD_PARAM(in_data, -1, &def));

    out_data->num_params = PARAM_COUNT;
    return err;
}

PF_Err ParameterHandler::HandleParamChanged(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    const PF_UserChangedParamExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    switch (extra->param_index) {
        case PARAM_RGB_CURVE:
        case PARAM_R_CURVE:
        case PARAM_G_CURVE:
        case PARAM_B_CURVE:
            seqData->needs_update = true;
            out_data->out_flags |= PF_OutFlag_FORCE_RERENDER;
            break;

        case PARAM_OPACITY:
            out_data->out_flags |= PF_OutFlag_REFRESH_UI;
            break;
    }

    return err;
}

PF_Err ParameterHandler::AddCurveParam(
    PF_InData* in_data,
    const char* name,
    PF_ParamFlags flags)
{
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    AEFX_CLR_STRUCT(def);
    PF_STRCPY(def.name, name);
    def.param_type = PF_Param_CURVE;
    def.u.pd.flags = flags;
    def.u.pd.dephault = 0;
    def.flags = PF_ParamFlag_SUPERVISE;
    ERR(PF_ADD_PARAM(in_data, -1, &def));

    return err;
}

PF_Err ParameterHandler::UpdateUI(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[])
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);

    if (!seqData) return PF_Err_INTERNAL_STRUCT_DAMAGED;

    // Enable/disable individual channel curves based on RGB curve state
    bool rgbEnabled = (params[PARAM_RGB_CURVE]->u.pd.value != 0);
    
    for (A_long i = PARAM_R_CURVE; i <= PARAM_B_CURVE; i++) {
        if (rgbEnabled) {
            params[i]->ui_flags &= ~PF_PUI_DISABLED;
        } else {
            params[i]->ui_flags |= PF_PUI_DISABLED;
        }
    }

    out_data->out_flags |= PF_OutFlag_REFRESH_UI;
    return err;
}