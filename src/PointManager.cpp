#include "PointManager.hpp"
#include <cmath>

PF_Err PointManager::FindNearestPoint(
    const CurveData* curve,
    const PF_Point& point,
    const PF_Rect& bounds,
    A_long* selectedIndex,
    PF_FpLong threshold)
{
    PF_Err err = PF_Err_NONE;
    *selectedIndex = -1;
    PF_FpLong minDistance = threshold;

    for (A_long i = 0; i < curve->num_points; i++) {
        // Convert curve point to screen coordinates
        PF_Point screenPoint = GetScreenCoordinates(curve->points[i], bounds);
        PF_FpLong distance = CalculateDistance(point, screenPoint);

        if (distance < minDistance) {
            minDistance = distance;
            *selectedIndex = i;
        }
    }

    return err;
}

PF_Err PointManager::ValidatePointMove(
    const CurveData* curve,
    A_long pointIndex,
    PF_FpLong newX,
    PF_FpLong newY,
    bool* isValid)
{
    PF_Err err = PF_Err_NONE;
    *isValid = false;

    if (pointIndex < 0 || pointIndex >= curve->num_points) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Check if new position would maintain x-order
    if (pointIndex > 0 && newX <= curve->points[pointIndex - 1].x) {
        return err;
    }
    if (pointIndex < curve->num_points - 1 && newX >= curve->points[pointIndex + 1].x) {
        return err;
    }

    // Validate coordinates are in range [0,1]
    if (newX < 0.0 || newX > 1.0 || newY < 0.0 || newY > 1.0) {
        return err;
    }

    *isValid = true;
    return err;
}

PF_Point PointManager::GetScreenCoordinates(
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

CurvePoint PointManager::GetCurveCoordinates(
    const PF_Point& screen_point,
    const PF_Rect& bounds)
{
    CurvePoint curve_point;
    A_long width = bounds.right - bounds.left;
    A_long height = bounds.bottom - bounds.top;

    curve_point.x = static_cast<PF_FpLong>(screen_point.h - bounds.left) / width;
    curve_point.y = 1.0 - static_cast<PF_FpLong>(screen_point.v - bounds.top) / height;

    // Clamp values to [0,1]
    curve_point.x = PF_MAX(0.0, PF_MIN(1.0, curve_point.x));
    curve_point.y = PF_MAX(0.0, PF_MIN(1.0, curve_point.y));

    return curve_point;
}

PF_FpLong PointManager::CalculateDistance(
    const PF_Point& p1,
    const PF_Point& p2)
{
    PF_FpLong dx = static_cast<PF_FpLong>(p2.h - p1.h);
    PF_FpLong dy = static_cast<PF_FpLong>(p2.v - p1.v);
    return sqrt(dx * dx + dy * dy);
}