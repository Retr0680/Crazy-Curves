#include "CurvesUI.hpp"
#include <algorithm>
#include <cstring>

const PF_Pixel CurvesUI::GRID_COLOR = {32, 32, 32, 255};
const PF_Pixel CurvesUI::CURVE_COLOR = {255, 255, 255, 255};
const PF_Pixel CurvesUI::POINT_COLOR = {255, 128, 0, 255};

CurvesUI::CurvesUI() 
    : activeChannel(ColorChannel::RGB)
    , selectedPoint(-1)
    , isDragging(false) {
    for (int i = 0; i < 5; ++i) {
        curves[i] = std::make_unique<CurveData>();
    }
}

CurvesUI::~CurvesUI() = default;

void CurvesUI::draw(unsigned char* buffer, int width, int height, int stride) {
    // Clear background
    std::memset(buffer, 0, stride * height);
    
    drawGrid(buffer, width, height, stride);
    drawCurve(buffer, width, height, stride);
    drawPoints(buffer, width, height, stride);
}

void CurvesUI::drawGrid(unsigned char* buffer, int width, int height, int stride) {
    // Draw grid lines
    for (int i = 0; i <= GRID_SIZE; ++i) {
        float t = static_cast<float>(i) / GRID_SIZE;
        int x = static_cast<int>(t * width);
        int y = static_cast<int>(t * height);
        
        // Draw horizontal line
        for (int px = 0; px < width; ++px) {
            buffer[y * stride + px * 4 + 0] = 64;  // R
            buffer[y * stride + px * 4 + 1] = 64;  // G
            buffer[y * stride + px * 4 + 2] = 64;  // B
            buffer[y * stride + px * 4 + 3] = 255; // A
        }
        
        // Draw vertical line
        for (int py = 0; py < height; ++py) {
            buffer[py * stride + x * 4 + 0] = 64;
            buffer[py * stride + x * 4 + 1] = 64;
            buffer[py * stride + x * 4 + 2] = 64;
            buffer[py * stride + x * 4 + 3] = 255;
        }
    }
}

void CurvesUI::drawCurve(unsigned char* buffer, int width, int height, int stride) {
    const auto& curve = curves[static_cast<int>(activeChannel)];
    
    // Draw curve segments
    for (int x = 0; x < width; ++x) {
        float t = static_cast<float>(x) / width;
        float y = curve->evaluate(t);
        int py = static_cast<int>((1.0f - y) * height);
        
        py = std::clamp(py, 0, height - 1);
        
        buffer[py * stride + x * 4 + 0] = 255;
        buffer[py * stride + x * 4 + 1] = 255;
        buffer[py * stride + x * 4 + 2] = 255;
        buffer[py * stride + x * 4 + 3] = 255;
    }
}

void CurvesUI::drawPoints(unsigned char* buffer, int width, int height, int stride) {
    const auto& curve = curves[static_cast<int>(activeChannel)];
    const auto& points = curve->getPoints();
    
    for (size_t i = 0; i < points.size(); ++i) {
        UIPoint screen = graphToScreen(points[i].x, points[i].y, width, height);
        
        // Draw point
        for (int dy = -POINT_RADIUS; dy <= POINT_RADIUS; ++dy) {
            for (int dx = -POINT_RADIUS; dx <= POINT_RADIUS; ++dx) {
                int px = static_cast<int>(screen.x) + dx;
                int py = static_cast<int>(screen.y) + dy;
                
                if (px >= 0 && px < width && py >= 0 && py < height) {
                    buffer[py * stride + px * 4 + 0] = i == selectedPoint ? 255 : 192;
                    buffer[py * stride + px * 4 + 1] = i == selectedPoint ? 255 : 192;
                    buffer[py * stride + px * 4 + 2] = i == selectedPoint ? 255 : 192;
                    buffer[py * stride + px * 4 + 3] = 255;
                }
            }
        }
    }
}

// ...implement remaining methods...