#pragma once
#include <cstdint>

struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
};

class DrawingUtils {
public:
    static void drawLine(uint8_t* buffer, int width, int height, int stride,
                        int x1, int y1, int x2, int y2, const Color& color);
                        
    static void drawRect(uint8_t* buffer, int width, int height, int stride,
                        int x, int y, int w, int h, const Color& color, bool filled = false);
                        
    static void drawCircle(uint8_t* buffer, int width, int height, int stride,
                          int x, int y, int radius, const Color& color, bool filled = false);
                          
    static void blendPixel(uint8_t* pixel, const Color& color);

private:
    static void plotLine(uint8_t* buffer, int width, int height, int stride,
                        int x1, int y1, int x2, int y2, const Color& color);
};