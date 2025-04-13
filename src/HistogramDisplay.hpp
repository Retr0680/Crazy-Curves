#pragma once
#include "AE_Effect.h"
#include <array>

class HistogramDisplay {
public:
    static const int HISTOGRAM_WIDTH = 256;
    static const int HISTOGRAM_HEIGHT = 100;
    
    HistogramDisplay();
    void update(PF_EffectWorld* input);
    void draw(PF_EffectWorld* world, const PF_Point& origin);
    void setChannel(int channel); // 0=RGB, 1=R, 2=G, 3=B

private:
    std::array<float, HISTOGRAM_WIDTH> histogramData;
    int currentChannel;
    bool isDirty;
    
    void calculateHistogram(PF_EffectWorld* input);
    void normalizeHistogram();
};