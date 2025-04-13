#include "CurveCache.hpp"

CurveCache::CurveCache() : isDirty(true) {
    invalidate();
}

void CurveCache::invalidate() {
    // Initialize with identity mapping
    for (int i = 0; i < CACHE_SIZE; ++i) {
        cache[i] = static_cast<float>(i) / (CACHE_SIZE - 1);
    }
    isDirty = true;
}

float CurveCache::getCachedValue(float input) const {
    float indexF = input * (CACHE_SIZE - 1);
    int index1 = static_cast<int>(indexF);
    int index2 = std::min(index1 + 1, CACHE_SIZE - 1);
    float t = indexF - index1;
    
    return cache[index1] * (1.0f - t) + cache[index2] * t;
}

void CurveCache::updateCache(const CurveData& curve) {
    if (!isDirty) return;

    for (int i = 0; i < CACHE_SIZE; ++i) {
        float x = static_cast<float>(i) / (CACHE_SIZE - 1);
        cache[i] = curve.evaluate(x);
    }
    
    isDirty = false;
}