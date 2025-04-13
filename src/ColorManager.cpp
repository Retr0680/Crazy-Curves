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

void ColorManager::processPixel8(const PF_Pixel8* input, PF_Pixel8* output, ColorChannel channel) {
    if (channel == ColorChannel::RGB) {
        output->red = lookupTables[0][input->red] * 255.0f;
        output->green = lookupTables[0][input->green] * 255.0f;
        output->blue = lookupTables[0][input->blue] * 255.0f;
    } else {
        int channelIndex = static_cast<int>(channel);
        output->red = (channel == ColorChannel::RED) ? 
            lookupTables[channelIndex][input->red] * 255.0f : input->red;
        output->green = (channel == ColorChannel::GREEN) ? 
            lookupTables[channelIndex][input->green] * 255.0f : input->green;
        output->blue = (channel == ColorChannel::BLUE) ? 
            lookupTables[channelIndex][input->blue] * 255.0f : input->blue;
    }
    output->alpha = input->alpha;
}

void ColorManager::processPixelFloat(const PF_PixelFloat* input, PF_PixelFloat* output, ColorChannel channel) {
    if (channel == ColorChannel::RGB) {
        output->red = interpolateLookup(lookupTables[0], input->red);
        output->green = interpolateLookup(lookupTables[0], input->green);
        output->blue = interpolateLookup(lookupTables[0], input->blue);
    } else {
        int channelIndex = static_cast<int>(channel);
        output->red = (channel == ColorChannel::RED) ? 
            interpolateLookup(lookupTables[channelIndex], input->red) : input->red;
        output->green = (channel == ColorChannel::GREEN) ? 
            interpolateLookup(lookupTables[channelIndex], input->green) : input->green;
        output->blue = (channel == ColorChannel::BLUE) ? 
            interpolateLookup(lookupTables[channelIndex], input->blue) : input->blue;
    }
    output->alpha = input->alpha;
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