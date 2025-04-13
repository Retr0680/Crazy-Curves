#include "DrawingUtils.hpp"
#include <algorithm>
#include <cmath>

namespace DrawingUtils {
    void drawLine(uint8_t* buffer, int width, int height, int stride,
                  int x1, int y1, int x2, int y2, const Color& color) {
        // Clip coordinates
        if (x1 < 0 || x1 >= width || y1 < 0 || y1 >= height ||
            x2 < 0 || x2 >= width || y2 < 0 || y2 >= height) {
            return;
        }
        
        plotLine(buffer, width, height, stride, x1, y1, x2, y2, color);
    }

    void drawRect(uint8_t* buffer, int width, int height, int stride,
                  int x, int y, int w, int h, const Color& color, bool filled) {
        if (filled) {
            for (int py = y; py < y + h; ++py) {
                for (int px = x; px < x + w; ++px) {
                    if (px >= 0 && px < width && py >= 0 && py < height) {
                        blendPixel(&buffer[py * stride + px * 4], color);
                    }
                }
            }
        } else {
            drawLine(buffer, width, height, stride, x, y, x + w, y, color);
            drawLine(buffer, width, height, stride, x + w, y, x + w, y + h, color);
            drawLine(buffer, width, height, stride, x + w, y + h, x, y + h, color);
            drawLine(buffer, width, height, stride, x, y + h, x, y, color);
        }
    }

    void drawCircle(uint8_t* buffer, int width, int height, int stride,
                    int x0, int y0, int radius, const Color& color, bool filled) {
        int x = radius;
        int y = 0;
        int err = 0;

        while (x >= y) {
            if (filled) {
                drawLine(buffer, width, height, stride, x0 - x, y0 + y, x0 + x, y0 + y, color);
                drawLine(buffer, width, height, stride, x0 - x, y0 - y, x0 + x, y0 - y, color);
                drawLine(buffer, width, height, stride, x0 - y, y0 + x, x0 + y, y0 + x, color);
                drawLine(buffer, width, height, stride, x0 - y, y0 - x, x0 + y, y0 - x, color);
            } else {
                blendPixel(&buffer[(y0 + y) * stride + (x0 + x) * 4], color);
                blendPixel(&buffer[(y0 + y) * stride + (x0 - x) * 4], color);
                blendPixel(&buffer[(y0 - y) * stride + (x0 + x) * 4], color);
                blendPixel(&buffer[(y0 - y) * stride + (x0 - x) * 4], color);
                blendPixel(&buffer[(y0 + x) * stride + (x0 + y) * 4], color);
                blendPixel(&buffer[(y0 + x) * stride + (x0 - y) * 4], color);
                blendPixel(&buffer[(y0 - x) * stride + (x0 + y) * 4], color);
                blendPixel(&buffer[(y0 - x) * stride + (x0 - y) * 4], color);
            }

            if (err <= 0) {
                y += 1;
                err += 2*y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2*x + 1;
            }
        }
    }

    void blendPixel(uint8_t* pixel, const Color& color) {
        float alpha = color.a / 255.0f;
        pixel[0] = static_cast<uint8_t>(pixel[0] * (1 - alpha) + color.r * alpha);
        pixel[1] = static_cast<uint8_t>(pixel[1] * (1 - alpha) + color.g * alpha);
        pixel[2] = static_cast<uint8_t>(pixel[2] * (1 - alpha) + color.b * alpha);
        pixel[3] = std::max(pixel[3], color.a);
    }

    void plotLine(uint8_t* buffer, int width, int height, int stride,
                  int x1, int y1, int x2, int y2, const Color& color) {
        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (true) {
            blendPixel(&buffer[y1 * stride + x1 * 4], color);
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }
}