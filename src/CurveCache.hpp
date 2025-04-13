#pragma once

#include <array>
#include <memory>
#include "CurveData.hpp"

class CurveCache {
public:
    static const int CACHE_SIZE = 256;

    CurveCache();
    void invalidate();
    float getCachedValue(float input) const;
    void updateCache(const CurveData& curve);

private:
    std::array<float, CACHE_SIZE> cache;
    bool isDirty;
};