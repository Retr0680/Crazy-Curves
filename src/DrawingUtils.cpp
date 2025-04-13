#include "DrawingUtils.hpp"

namespace DrawingUtils {
    void drawRect(PF_EffectWorld* world, const PF_Rect& rect, const PF_Pixel& color) {
        // Draw rectangle outline
        drawLine(world, rect.left, rect.top, rect.right, rect.top, color);
        drawLine(world, rect.right, rect.top, rect.right, rect.bottom, color);
        drawLine(world, rect.right, rect.bottom, rect.left, rect.bottom, color);
        drawLine(world, rect.left, rect.bottom, rect.left, rect.top, color);
    }

    void drawLine(PF_EffectWorld* world, int x1, int y1, int x2, int y2, const PF_Pixel& color) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            if (x1 >= 0 && x1 < world->width && y1 >= 0 && y1 < world->height) {
                PF_Pixel* pixel = (PF_Pixel*)((char*)world->data + y1 * world->rowbytes + x1 * sizeof(PF_Pixel));
                *pixel = color;
            }

            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    }

    bool pointInRect(const PF_Point& point, const PF_Rect& rect) {
        return point.x >= rect.left && point.x <= rect.right &&
               point.y >= rect.top && point.y <= rect.bottom;
    }

    PF_Rect offsetRect(const PF_Rect& rect, const PF_Point& offset) {
        return {
            rect.left + offset.x,
            rect.top + offset.y,
            rect.right + offset.x,
            rect.bottom + offset.y
        };
    }
}