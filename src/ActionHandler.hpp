#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ActionHandler {
public:
    static PF_Err HandleNewSequence(
        PF_InData* in_data,
        PF_OutData* out_data);
        
    static PF_Err HandleResetup(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output);
        
    static PF_Err HandleFlattenSequence(
        PF_InData* in_data,
        PF_OutData* out_data);
        
    static PF_Err HandleSequenceSetdown(
        PF_InData* in_data,
        PF_OutData* out_data);

private:
    static PF_Err InitializeCurves(SequenceData* seqData);
    static PF_Err CleanupSequenceData(SequenceData* seqData);
};