#include "HistogramDisplay.hpp"
#include "DrawingUtils.hpp"
#include <algorithm>

HistogramDisplay::HistogramDisplay() 
    : currentChannel(0)
    , isDirty(true) {
    histogramData.fill(0.0f);
}

void HistogramDisplay::update(PF_EffectWorld* input) {
    calculateHistogram(input);
    normalizeHistogram();
    isDirty = false;
}

void HistogramDisplay::draw(PF_EffectWorld* world, const PF_Point& origin) {
    // Draw background
    PF_Rect bounds = {
        origin.x,
        origin.y,
        origin.x + HISTOGRAM_WIDTH,
        origin.y + HISTOGRAM_HEIGHT
    };
    DrawingUtils::drawRect(world, bounds, PF_Pixel{32, 32, 32, 255});
    
    // Draw histogram bars
    PF_Pixel color;
    switch (currentChannel) {
        case 1: color = PF_Pixel{255, 0, 0, 255}; break;  // Red
        case 2: color = PF_Pixel{0, 255, 0, 255}; break;  // Green
        case 3: color = PF_Pixel{0, 0, 255, 255}; break;  // Blue
        default: color = PF_Pixel{255, 255, 255, 255};    // RGB
    }
    
    for (int i = 0; i < HISTOGRAM_WIDTH; ++i) {
        int height = static_cast<int>(histogramData[i] * HISTOGRAM_HEIGHT);
        if (height > 0) {
            PF_Point start = {origin.x + i, origin.y + HISTOGRAM_HEIGHT};
            PF_Point end = {origin.x + i, origin.y + HISTOGRAM_HEIGHT - height};
            DrawingUtils::drawLine(world, start.x, start.y, end.x, end.y, color);
        }
    }
}

void HistogramDisplay::setChannel(int channel) {
    if (currentChannel != channel) {
        currentChannel = channel;
        isDirty = true;
    }
}

void HistogramDisplay::calculateHistogram(PF_EffectWorld* input) {
    histogramData.fill(0.0f);
    
    PF_Pixel* pixelData = reinterpret_cast<PF_Pixel*>(input->data);
    int rowBytes = input->rowbytes / sizeof(PF_Pixel);
    
    for (int y = 0; y < input->height; ++y) {
        for (int x = 0; x < input->width; ++x) {
            PF_Pixel pixel = pixelData[y * rowBytes + x];
            switch (currentChannel) {
                case 1: histogramData[pixel.red]++; break;
                case 2: histogramData[pixel.green]++; break;
                case 3: histogramData[pixel.blue]++; break;
                default: {
                    float luminance = 0.299f * pixel.red + 
                                    0.587f * pixel.green + 
                                    0.114f * pixel.blue;
                    histogramData[static_cast<int>(luminance)]++;
                }
            }
        }
    }
}

void HistogramDisplay::normalizeHistogram() {
    float maxValue = *std::max_element(histogramData.begin(), histogramData.end());
    if (maxValue > 0) {
        for (float& value : histogramData) {
            value /= maxValue;
        }
    }
}