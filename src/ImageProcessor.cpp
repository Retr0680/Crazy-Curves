#pragma once
#include "ImageProcessor.hpp"
#include <algorithm>

ImageProcessor::ImageProcessor() : channelManager(nullptr) {}

void ImageProcessor::processImage(uint8_t* data, int width, int height, int stride) {
    if (!channelManager) return;
    
    updateCaches();
    
    for (int y = 0; y < height; ++y) {
        uint8_t* row = data + y * stride;
        for (int x = 0; x < width; ++x) {
            processPixel(row + x * 4);
        }
    }
}

void ImageProcessor::updateCaches() {
    for (size_t i = 0; i < caches.size(); ++i) {
        auto channel = static_cast<ChannelManager::Channel>(i);
        if (auto curve = channelManager->getCurve(channel)) {
            caches[i].updateCache(*curve);
        }
    }
}

void ImageProcessor::processPixel(uint8_t* pixel) {
    // Process RGB channels
    for (int i = 0; i < 3; ++i) {
        float value = pixel[i] / 255.0f;
        
        // Apply RGB curve first
        value = processChannel(value * 255.0f, ChannelManager::Channel::RGB) / 255.0f;
        
        // Then apply individual channel curve
        auto channel = static_cast<ChannelManager::Channel>(i + 1); // RED = 1, GREEN = 2, BLUE = 3
        value = processChannel(value * 255.0f, channel) / 255.0f;
        
        pixel[i] = static_cast<uint8_t>(std::clamp(value * 255.0f, 0.0f, 255.0f));
    }
    
    // Process Alpha channel separately
    float alpha = pixel[3] / 255.0f;
    alpha = processChannel(alpha * 255.0f, ChannelManager::Channel::ALPHA) / 255.0f;
    pixel[3] = static_cast<uint8_t>(std::clamp(alpha * 255.0f, 0.0f, 255.0f));
}

float ImageProcessor::processChannel(uint8_t value, ChannelManager::Channel channel) {
    return caches[static_cast<size_t>(channel)].getCachedValue(value / 255.0f) * 255.0f;
}