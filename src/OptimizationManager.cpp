#include "OptimizationManager.hpp"
#include <algorithm>

PF_Err OptimizationManager::OptimizeCurveEvaluation(
    const CurveData* curve,
    PF_Boolean useCache)
{
    PF_Err err = PF_Err_NONE;

    if (!curve) return PF_Err_BAD_CALLBACK_PARAM;

    // Create optimized lookup table if caching is enabled
    if (useCache && curve->dirty) {
        PF_FpLong* lut = new PF_FpLong[LUT_SIZE];
        if (!lut) return PF_Err_OUT_OF_MEMORY;

        ERR(PrecomputeLUT(curve, lut, LUT_SIZE));

        delete[] lut;
    }

    return err;
}

PF_Err OptimizationManager::OptimizeMemory(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Calculate optimal block size based on available memory
    A_long width = in_data->width;
    A_long height = in_data->height;
    A_long optimalBlocks = (width * height) / BLOCK_SIZE;

    // Set output flags for memory optimization
    out_data->out_flags |= PF_OutFlag_PIX_INDEPENDENT;
    out_data->out_flags2 |= PF_OutFlag2_FLOAT_COLOR_AWARE;

    return err;
}

PF_Err OptimizationManager::PrecomputeLUT(
    const CurveData* curve,
    PF_FpLong* lut,
    A_long size)
{
    PF_Err err = PF_Err_NONE;

    for (A_long i = 0; i < size; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (size - 1);
        
        // Find segment containing x
        A_long segIndex = 0;
        for (; segIndex < curve->num_points - 1; segIndex++) {
            if (x <= curve->points[segIndex + 1].x) break;
        }

        // Interpolate
        PF_FpLong x0 = curve->points[segIndex].x;
        PF_FpLong y0 = curve->points[segIndex].y;
        PF_FpLong x1 = curve->points[segIndex + 1].x;
        PF_FpLong y1 = curve->points[segIndex + 1].y;

        PF_FpLong t = (x - x0) / (x1 - x0);
        lut[i] = y0 + t * (y1 - y0);
    }

    return err;
}

PF_Err OptimizationManager::OptimizePointOrder(CurveData* curve)
{
    PF_Err err = PF_Err_NONE;

    // Sort points by x coordinate
    for (A_long i = 0; i < curve->num_points - 1; i++) {
        for (A_long j = 0; j < curve->num_points - i - 1; j++) {
            if (curve->points[j].x > curve->points[j + 1].x) {
                std::swap(curve->points[j], curve->points[j + 1]);
            }
        }
    }

    return err;
}

PF_Err OptimizationManager::RemoveRedundantPoints(
    CurveData* curve,
    PF_FpLong threshold)
{
    PF_Err err = PF_Err_NONE;

    if (curve->num_points < 3) return err;

    A_long writeIndex = 1;
    for (A_long i = 1; i < curve->num_points - 1; i++) {
        // Check if point is non-redundant
        PF_FpLong dx1 = curve->points[i].x - curve->points[i-1].x;
        PF_FpLong dy1 = curve->points[i].y - curve->points[i-1].y;
        PF_FpLong dx2 = curve->points[i+1].x - curve->points[i].x;
        PF_FpLong dy2 = curve->points[i+1].y - curve->points[i].y;

        PF_FpLong angle = abs(atan2(dy2, dx2) - atan2(dy1, dx1));
        
        if (angle > threshold) {
            curve->points[writeIndex++] = curve->points[i];
        }
    }

    // Always keep last point
    curve->points[writeIndex++] = curve->points[curve->num_points - 1];
    curve->num_points = writeIndex;
    curve->dirty = true;

    return err;
}