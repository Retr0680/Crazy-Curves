#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "CrazyCurves.h"

class SequenceDataManager {
public:
    static PF_Err Allocate(
        PF_InData* in_data,
        PF_OutData* out_data);

    static PF_Err Dispose(
        PF_InData* in_data,
        PF_OutData* out_data);

    static PF_Err Flatten(
        PF_InData* in_data,
        PF_OutData* out_data);

    static PF_Err Setup(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_ParamDef* params[],
        PF_LayerDef* output);

private:
    static PF_Err InitializeCurves(SequenceData* seqData);
    static void CleanupCurves(SequenceData* seqData);
};