#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class GPUProcessingManager {
public:
    static GPUProcessingManager& getInstance() {
        static GPUProcessingManager instance;
        return instance;
    }

    PF_Err InitializeGPU(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err ProcessFrameGPU(
        PF_InData* in_data,
        PF_OutData* out_data,
        PF_GPUWorld* input,
        PF_GPUWorld* output,
        const CurveData* curves);

    PF_Err ShutdownGPU();

private:
    struct GPUResources {
        PF_GPUHandle curveBuffer;
        PF_GPUHandle lookupTexture;
        bool initialized;
    };

    GPUResources resources;
    
    PF_Err CreateGPUBuffers(
        PF_InData* in_data,
        const CurveData* curves);

    PF_Err UpdateLookupTexture(
        PF_InData* in_data,
        const CurveData* curves);
};