#include "CacheManager.hpp"

PF_Err CacheManager::initializeCache(PF_EffectWorld* input, PF_EffectWorld* output) 
{
    PF_Err err = PF_Err_NONE;

    // Clear existing cache
    for (A_long c = 0; c < NUM_CHANNELS; c++) {
        for (A_long i = 0; i < CACHE_SIZE; i++) {
            lookupTables[c][i] = static_cast<PF_FpLong>(i) / (CACHE_SIZE - 1);
        }
    }

    isValid = true;
    return err;
}

PF_Err CacheManager::updateCurveLookup(const CurveData* curve, A_long channel) 
{
    if (channel < 0 || channel >= NUM_CHANNELS)
        return PF_Err_BAD_CALLBACK_PARAM;

    return buildLookupTable(curve, lookupTables[channel]);
}

void CacheManager::invalidateCache() 
{
    isValid = false;
}

PF_Err CacheManager::buildLookupTable(const CurveData* curve, PF_FpLong* table) 
{
    PF_Err err = PF_Err_NONE;

    if (!curve || !table)
        return PF_Err_BAD_CALLBACK_PARAM;

    // Build lookup table with curve interpolation
    for (A_long i = 0; i < CACHE_SIZE; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (CACHE_SIZE - 1);
        
        // Find segment containing x
        A_long segIndex = 0;
        for (; segIndex < curve->num_points - 1; segIndex++) {
            if (x <= curve->points[segIndex + 1].x)
                break;
        }

        // Interpolate between points
        PF_FpLong x0 = curve->points[segIndex].x;
        PF_FpLong y0 = curve->points[segIndex].y;
        PF_FpLong x1 = curve->points[segIndex + 1].x;
        PF_FpLong y1 = curve->points[segIndex + 1].y;

        PF_FpLong t = (x - x0) / (x1 - x0);
        table[i] = y0 + t * (y1 - y0);
    }

    return err;
}