#pragma once

#include <array>
#include <memory>
#include "CurvesData.h"

class CurveCache {
public:
    static const int CACHE_SIZE = 256;
    
    CurveCache();
    void updateCache(const CurveData& curve);
    float getCachedValue(float x) const;
    void invalidate();
    
private:
    std::array<float, CACHE_SIZE> cache;
    bool isDirty;
    
    void buildCache(const CurveData& curve);
};