#pragma once

#include <array>
#include <cstdint>

class CurveCache {
public:
    static const int CACHE_SIZE = 256;
    CurveCache() : currentTimestamp(0) {}

private:
    struct CacheEntry {
        std::array<float, CACHE_SIZE> values;
        uint64_t timestamp;
    };
    
    static const int CACHE_CHANNELS = 4;
    std::array<CacheEntry, CACHE_CHANNELS> channelCaches;
    uint64_t currentTimestamp;
    
    void optimizeCacheUsage();
};