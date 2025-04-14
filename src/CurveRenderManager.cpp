#include "CurveRenderManager.hpp"

PF_Err CurveRenderManager::RenderCurve(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve,
    const PF_Pixel8& color)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Draw curve segments
    for (A_long i = 0; i < curve->num_points - 1; i++) {
        PF_Point p1 = WorldToScreen(curve->points[i], bounds);
        PF_Point p2 = WorldToScreen(curve->points[i + 1], bounds);
        
        ERR(DrawLine(world, p1.h, p1.v, p2.h, p2.v, color));
        if (err) break;
    }

    return err;
}

PF_Err CurveRenderManager::RenderControlPoints(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const CurveData* curve,
    const PF_Pixel8& color,
    A_long selectedPoint)
{
    PF_Err err = PF_Err_NONE;

    // Draw all control points
    for (A_long i = 0; i < curve->num_points; i++) {
        PF_Point screenPt = WorldToScreen(curve->points[i], bounds);
        A_long radius = (i == selectedPoint) ? SELECTED_POINT_RADIUS : POINT_RADIUS;
        
        ERR(DrawPoint(world, screenPt.h, screenPt.v, radius, color, i == selectedPoint));
        if (err) break;
    }

    return err;
}

PF_Err CurveRenderManager::RenderGrid(
    PF_InData* in_data,
    PF_EffectWorld* world,
    const PF_Rect& bounds,
    const PF_Pixel8& color)
{
    PF_Err err = PF_Err_NONE;

    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;
    A_long stepX = width / GRID_DIVISIONS;
    A_long stepY = height / GRID_DIVISIONS;

    // Draw vertical lines
    for (A_long x = bounds.left; x <= bounds.right; x += stepX) {
        ERR(DrawLine(world, x, bounds.top, x, bounds.bottom, color));
        if (err) break;
    }

    // Draw horizontal lines
    if (!err) {
        for (A_long y = bounds.top; y <= bounds.bottom; y += stepY) {
            ERR(DrawLine(world, bounds.left, y, bounds.right, y, color));
            if (err) break;
        }
    }

    return err;
}

PF_Err CurveRenderManager::DrawLine(
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
        ERR(DrawPoint(world, x1, y1, 1, color, true));
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

PF_Err CurveRenderManager::DrawPoint(
    PF_EffectWorld* world,
    A_long x, A_long y,
    A_long radius,
    const PF_Pixel8& color,
    bool filled)
{
    PF_Err err = PF_Err_NONE;

    // Check bounds
    if (x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return err;
    }

    if (filled) {
        // Draw filled circle using midpoint circle algorithm
        A_long f = 1 - radius;
        A_long ddF_x = 0;
        A_long ddF_y = -2 * radius;
        A_long cx = 0;
        A_long cy = radius;

        PF_Pixel8* pixel = (PF_Pixel8*)((char*)world->data + 
            y * world->rowbytes + x * sizeof(PF_Pixel8));
        *pixel = color;

        while (cx < cy) {
            if (f >= 0) {
                cy--;
                ddF_y += 2;
                f += ddF_y;
            }
            cx++;
            ddF_x += 2;
            f += ddF_x + 1;

            for (A_long px = -cx; px <= cx; px++) {
                for (A_long py = -cy; py <= cy; py++) {
                    if (px*px + py*py <= radius*radius) {
                        A_long px1 = x + px;
                        A_long py1 = y + py;
                        if (px1 >= 0 && px1 < world->width && 
                            py1 >= 0 && py1 < world->height) {
                            pixel = (PF_Pixel8*)((char*)world->data + 
                                py1 * world->rowbytes + px1 * sizeof(PF_Pixel8));
                            *pixel = color;
                        }
                    }
                }
            }
        }
    } else {
        // Draw circle outline
        A_long f = 1 - radius;
        A_long ddF_x = 1;
        A_long ddF_y = -2 * radius;
        A_long cx = 0;
        A_long cy = radius;

        while (cx < cy) {
            if (f >= 0) {
                cy--;
                ddF_y += 2;
                f += ddF_y;
            }
            cx++;
            ddF_x += 2;
            f += ddF_x;

            PF_Pixel8* pixel;
            if (x + cx < world->width && y + cy < world->height) {
                pixel = (PF_Pixel8*)((char*)world->data + 
                    (y + cy) * world->rowbytes + (x + cx) * sizeof(PF_Pixel8));
                *pixel = color;
            }
            // Continue for other octants...
        }
    }

    return err;
}

PF_Point CurveRenderManager::WorldToScreen(
    const CurvePoint& point,
    const PF_Rect& bounds)
{
    PF_Point screen;
    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;

    screen.h = bounds.left + static_cast<A_long>(point.x * width);
    screen.v = bounds.bottom - static_cast<A_long>(point.y * height);

    return screen;
}