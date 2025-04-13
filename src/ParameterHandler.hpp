#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ParameterHandler {
public:
    static PF_Err SetupParams(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[]);

    static PF_Err HandleParamChanged(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output,
        const PF_UserChangedParamExtra* extra);

private:
    static PF_Err AddCurveParam(
        PF_InData* in_data,
        const char* name,
        PF_ParamFlags flags);

    static PF_Err UpdateUI(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[]);
};