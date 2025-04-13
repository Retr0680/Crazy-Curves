#pragma once
#include "ChannelManager.hpp"
#include "CurveCache.hpp"

class ImageProcessor {
public:
    ImageProcessor();
    
    void processImage(uint8_t* data, int width, int height, int stride);
    void setChannelManager(ChannelManager* manager) { channelManager = manager; }
    void updateCaches();

private:
    ChannelManager* channelManager;
    std::array<CurveCache, static_cast<size_t>(ChannelManager::Channel::COUNT)> caches;
    
    void processPixel(uint8_t* pixel);
    float processChannel(uint8_t value, ChannelManager::Channel channel);
};