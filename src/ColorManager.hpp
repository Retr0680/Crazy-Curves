#pragma once
#include <array>

enum class ColorChannel {
    RGB,
    RED,
    GREEN,
    BLUE,
    ALPHA
};

class ColorManager {
public:
    ColorManager();
    
    void processPixel8(const unsigned char* input, unsigned char* output, ColorChannel channel);
    void processPixelFloat(const float* input, float* output, ColorChannel channel);
    
    void setCurveValue(ColorChannel channel, float input, float output);
    float getCurveValue(ColorChannel channel, float input) const;

private:
    static const int LOOKUP_SIZE = 256;
    std::array<std::array<float, LOOKUP_SIZE>, 5> lookupTables; // One for each channel + RGB
    
    void initializeLookupTables();
    float interpolateLookup(const std::array<float, LOOKUP_SIZE>& table, float value) const;
};