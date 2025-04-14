#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class GPUHandler {
public:
    static PF_Err SetupGPU(
        PF_InData* in_data,
        PF_OutData* out_data);

    static PF_Err RenderGPU(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_GPUWorld* input,
        PF_GPUWorld* output,
        const CurveData* curves);

private:
    static PF_Err BuildGPULookupTextures(
        PF_InData* in_data,
        const CurveData* curves);
        
    static PF_Err CreateGPUKernels();
};