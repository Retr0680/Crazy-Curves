#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class BatchProcessor {
public:
    static BatchProcessor& getInstance() {
        static BatchProcessor instance;
        return instance;
    }

    PF_Err ProcessImageBatch(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves,
        A_long startLine,
        A_long endLine);

    PF_Err QueueBatchOperation(
        PF_InData* in_data,
        const CurveData* curves,
        A_long startFrame,
        A_long endFrame);

private:
    struct BatchTask {
        A_long startLine;
        A_long endLine;
        const CurveData* curves;
    };

    std::vector<BatchTask> taskQueue;
    static const A_long BATCH_SIZE = 64;

    PF_Err ProcessPixelBatch8(
        PF_Pixel8* inP,
        PF_Pixel8* outP,
        const CurveData* curves,
        A_long numPixels);

    PF_Err ProcessPixelBatchFloat(
        PF_PixelFloat* inP,
        PF_PixelFloat* outP,
        const CurveData* curves,
        A_long numPixels);
};