#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class OptimizationManager {
public:
    static OptimizationManager& getInstance() {
        static OptimizationManager instance;
        return instance;
    }

    PF_Err OptimizeCurveEvaluation(
        const CurveData* curve,
        PF_Boolean useCache = true);

    PF_Err OptimizeMemory(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err PrecomputeLUT(
        const CurveData* curve,
        PF_FpLong* lut,
        A_long size);

private:
    OptimizationManager() = default;
    static const A_long LUT_SIZE = 256;
    static const A_long BLOCK_SIZE = 64;

    struct OptimizationMetrics {
        PF_FpLong evaluationTime;
        A_long cacheHits;
        A_long cacheMisses;
    } metrics;

    PF_Err OptimizePointOrder(CurveData* curve);
    PF_Err RemoveRedundantPoints(CurveData* curve, PF_FpLong threshold = 0.001);
};