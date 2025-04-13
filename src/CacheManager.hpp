#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class CacheManager {
public:
    static CacheManager& getInstance() {
        static CacheManager instance;
        return instance;
    }

    PF_Err initializeCache(PF_EffectWorld* input, PF_EffectWorld* output);
    PF_Err updateCurveLookup(const CurveData* curve, A_long channel);
    void invalidateCache();
    bool isCacheValid() const { return isValid; }

private:
    CacheManager() : isValid(false) {}
    
    static const A_long CACHE_SIZE = 256;
    static const A_long NUM_CHANNELS = 4; // RGB + Master

    PF_FpLong lookupTables[NUM_CHANNELS][CACHE_SIZE];
    bool isValid;

    PF_Err buildLookupTable(const CurveData* curve, PF_FpLong* table);
};