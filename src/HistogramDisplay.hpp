#pragma once
#include <array>
#include "ColorManager.hpp"

class HistogramDisplay {
public:
    static const int HISTOGRAM_WIDTH = 256;
    static const int HISTOGRAM_HEIGHT = 100;
    static const int BORDER_SIZE = 1;
    
    HistogramDisplay();
    
    void update(const uint8_t* imageData, int width, int height, int stride);
    void draw(uint8_t* buffer, int width, int height, int stride,
             const PF_Point& origin);
             
    void setChannel(ColorChannel channel) { currentChannel = channel; }
    void setScale(float scale) { displayScale = scale; }

private:
    std::array<float, HISTOGRAM_WIDTH> histogramData;
    ColorChannel currentChannel;
    float displayScale;
    bool isDirty;
    
    void calculateHistogram(const uint8_t* imageData, int width, int height, int stride);
    void normalizeHistogram();
};