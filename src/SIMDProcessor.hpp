#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <immintrin.h>

class SIMDProcessor {
public:
    static PF_Err ProcessPixelBlock8(
        const PF_Pixel8* inP,
        PF_Pixel8* outP,
        const CurveData* curves,
        A_long numPixels);

    static PF_Err ProcessPixelBlockFloat(
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP,
        const CurveData* curves,
        A_long numPixels);

private:
    static const A_long SIMD_BLOCK_SIZE = 8;

    static __m256 EvaluateCurveSIMD(
        const CurveData* curve,
        __m256 x);

    static PF_Err InitializeLookupTables(
        const CurveData* curves);
};