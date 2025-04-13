#include "CurveCache.hpp"

CurveCache::CurveCache() : isDirty(true), currentTimestamp(0) {
    cache.fill(0.0f);
}

void CurveCache::updateCache(const CurveData& curve) {
    if (isDirty) {
        buildCache(curve);
        isDirty = false;
    }
}

float CurveCache::getCachedValue(float x) const {
    int index = static_cast<int>(x * (CACHE_SIZE - 1));
    index = std::max(0, std::min(CACHE_SIZE - 1, index));
    return cache[index];
}

void CurveCache::invalidate() {
    isDirty = true;
}

void CurveCache::buildCache(const CurveData& curve) {
    for (int i = 0; i < CACHE_SIZE; ++i) {
        float x = static_cast<float>(i) / (CACHE_SIZE - 1);
        cache[i] = curve.evaluate(x);
    }
}

void CurveCache::optimizeCacheUsage() {
    currentTimestamp++;
    
    // Clean up old cache entries if timestamp overflows
    if (currentTimestamp == UINT64_MAX) {
        for (auto& cache : channelCaches) {
            cache.timestamp = 0;
        }
        currentTimestamp = 1;
    }
}