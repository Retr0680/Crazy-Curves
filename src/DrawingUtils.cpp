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

    PF_Err DrawRect(
        PF_EffectWorld* world,
        const PF_Rect& rect,
        const PF_Pixel8& color)
    {
        PF_Err err = PF_Err_NONE;
        AEGP_SuiteHandler suites(in_data->pica_basicP);

        // Draw horizontal lines
        for (A_long x = rect.left; x <= rect.right; x++) {
            PlotPixel(world, x, rect.top, color);
            PlotPixel(world, x, rect.bottom, color);
        }

        // Draw vertical lines
        for (A_long y = rect.top; y <= rect.bottom; y++) {
            PlotPixel(world, rect.left, y, color);
            PlotPixel(world, rect.right, y, color);
        }

        return err;
    }

    PF_Err DrawLine(
        PF_EffectWorld* world,
        A_long x1, A_long y1,
        A_long x2, A_long y2,
        const PF_Pixel8& color)
    {
        PF_Err err = PF_Err_NONE;

        // Bresenham's line algorithm
        A_long dx = abs(x2 - x1);
        A_long dy = abs(y2 - y1);
        A_long sx = (x1 < x2) ? 1 : -1;
        A_long sy = (y1 < y2) ? 1 : -1;
        A_long err2 = dx - dy;

        while (true) {
            PlotPixel(world, x1, y1, color);
            if (x1 == x2 && y1 == y2) break;
            A_long e2 = 2 * err2;
            if (e2 > -dy) {
                err2 -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err2 += dx;
                y1 += sy;
            }
        }

        return err;
    }

    PF_Err DrawText(
        PF_InData* in_data,
        PF_EffectWorld* world,
        const std::string& text,
        const PF_Point& position,
        const PF_Pixel8& color)
    {
        PF_Err err = PF_Err_NONE;
        AEGP_SuiteHandler suites(in_data->pica_basicP);

        ERR(suites.ANSICallbacksSuite1()->draw_string(
            in_data->effect_win,
            const_cast<char*>(text.c_str()),
            position.h,
            position.v,
            &color));

        return err;
    }

    PF_Err DrawPoint(
        PF_EffectWorld* world,
        const PF_Point& center,
        A_long radius,
        const PF_Pixel8& color,
        bool filled)
    {
        PF_Err err = PF_Err_NONE;

        if (filled) {
            // Draw filled circle
            for (A_long y = -radius; y <= radius; y++) {
                for (A_long x = -radius; x <= radius; x++) {
                    if (x*x + y*y <= radius*radius) {
                        PlotPixel(world, center.h + x, center.v + y, color);
                    }
                }
            }
        } else {
            // Draw circle outline using midpoint circle algorithm
            A_long x = radius;
            A_long y = 0;
            A_long err2 = 0;

            while (x >= y) {
                PlotPixel(world, center.h + x, center.v + y, color);
                PlotPixel(world, center.h + y, center.v + x, color);
                PlotPixel(world, center.h - y, center.v + x, color);
                PlotPixel(world, center.h - x, center.v + y, color);
                PlotPixel(world, center.h - x, center.v - y, color);
                PlotPixel(world, center.h - y, center.v - x, color);
                PlotPixel(world, center.h + y, center.v - x, color);
                PlotPixel(world, center.h + x, center.v - y, color);

                y++;
                err2 += 2*y + 1;
                if (2*err2 > 2*x - 1) {
                    x--;
                    err2 -= 2*x + 1;
                }
            }
        }

        return err;
    }

    bool PointInRect(
        const PF_Point& point,
        const PF_Rect& rect)
    {
        return point.h >= rect.left && point.h <= rect.right &&
               point.v >= rect.top && point.v <= rect.bottom;
    }

    void PlotPixel(
        PF_EffectWorld* world,
        A_long x, A_long y,
        const PF_Pixel8& color)
    {
        if (x >= 0 && x < world->width && y >= 0 && y < world->height) {
            PF_Pixel8* pixel = (PF_Pixel8*)((char*)world->data + 
                y * world->rowbytes + x * sizeof(PF_Pixel8));
            *pixel = color;
        }
    }
}