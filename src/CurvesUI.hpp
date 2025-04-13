#pragma once

#include <memory>
#include "AE_Effect.h"
#include "ChannelManager.hpp"
#include "PreviewWindow.hpp"
#include "CurveHistory.hpp"
#include "CurveData.hpp"
#include "ColorManager.hpp"
#include "PresetManager.hpp"

struct UIPoint {
    float x, y;
    bool selected;
    bool hover;
};

class CurvesUI {
public:
    CurvesUI();
    ~CurvesUI();

    void draw(unsigned char* buffer, int width, int height, int stride);
    bool handleMouseMove(int x, int y);
    bool handleMouseDown(int x, int y, bool isShiftDown);
    bool handleMouseUp(int x, int y);
    
    void setActiveChannel(ColorChannel channel);
    void applyPreset(const std::string& presetName);
    
private:
    static const int GRID_SIZE = 8;
    static const int POINT_RADIUS = 4;
    
    std::unique_ptr<CurveData> curves[5];  // One for each channel + RGB
    ColorManager colorManager;
    PresetManager presetManager;
    ColorChannel activeChannel;
    int selectedPoint;
    bool isDragging;
    
    void drawGrid(unsigned char* buffer, int width, int height, int stride);
    void drawCurve(unsigned char* buffer, int width, int height, int stride);
    void drawPoints(unsigned char* buffer, int width, int height, int stride);
    
    UIPoint screenToGraph(int x, int y, int width, int height);
    UIPoint graphToScreen(float x, float y, int width, int height);
};