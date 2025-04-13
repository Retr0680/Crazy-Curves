#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class KeyboardHandler {
public:
    static PF_Err HandleKeyEvent(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_KeyDownEvent* key_event);

private:
    static PF_Err HandleDelete(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData);

    static PF_Err HandleModifiers(
        PF_InData* in_data,
        PF_OutData* out_data,
        SequenceData* seqData,
        const PF_KeyDownEvent* key_event); 
};