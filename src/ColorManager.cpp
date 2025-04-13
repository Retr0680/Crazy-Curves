#include "ColorManager.hpp"
#include <algorithm>

ColorManager::ColorManager() {
    initializeLookupTables();
}

void ColorManager::initializeLookupTables() {
    for (auto& table : lookupTables) {
        for (int i = 0; i < LOOKUP_SIZE; ++i) {
            table[i] = static_cast<float>(i) / (LOOKUP_SIZE - 1);
        }
    }
}

void ColorManager::processPixel8(const unsigned char* input, unsigned char* output, ColorChannel channel) {
    int channelIndex = static_cast<int>(channel);
    
    for (int i = 0; i < 4; ++i) {
        if (channel == ColorChannel::RGB || i == channelIndex) {
            float value = input[i] / 255.0f;
            output[i] = static_cast<unsigned char>(getCurveValue(channel, value) * 255.0f);
        } else {
            output[i] = input[i];
        }
    }
}

void ColorManager::processPixelFloat(const float* input, float* output, ColorChannel channel) {
    int channelIndex = static_cast<int>(channel);
    
    for (int i = 0; i < 4; ++i) {
        if (channel == ColorChannel::RGB || i == channelIndex) {
            output[i] = getCurveValue(channel, input[i]);
        } else {
            output[i] = input[i];
        }
    }
}

void ColorManager::setCurveValue(ColorChannel channel, float input, float output) {
    int index = std::min(static_cast<int>(input * (LOOKUP_SIZE - 1)), LOOKUP_SIZE - 1);
    lookupTables[static_cast<int>(channel)][index] = output;
}

float ColorManager::getCurveValue(ColorChannel channel, float input) const {
    return interpolateLookup(lookupTables[static_cast<int>(channel)], input);
}

float ColorManager::interpolateLookup(const std::array<float, LOOKUP_SIZE>& table, float value) const {
    float indexF = value * (LOOKUP_SIZE - 1);
    int index1 = static_cast<int>(indexF);
    int index2 = std::min(index1 + 1, LOOKUP_SIZE - 1);
    float t = indexF - index1;
    
    return table[index1] * (1.0f - t) + table[index2] * t;
}