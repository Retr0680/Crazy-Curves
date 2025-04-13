#pragma once

#include <array>
#include <cstdint>

class CurveCache {
public:
    static const int CACHE_SIZE = 256;
    CurveCache();
    void updateCache(const CurveData& curve);
    float getCachedValue(float x) const;
    void invalidate();

private:
    struct CacheEntry {
        std::array<float, CACHE_SIZE> values;
        uint64_t timestamp;
    };
    
    static const int CACHE_CHANNELS = 4;
    std::array<CacheEntry, CACHE_CHANNELS> channelCaches;
    uint64_t currentTimestamp;
    bool isDirty;
    std::array<float, CACHE_SIZE> cache;
    
    void buildCache(const CurveData& curve);
    void optimizeCacheUsage();
};