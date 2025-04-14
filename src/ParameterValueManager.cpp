#include "ParameterValueManager.hpp"
#include "ErrorHandler.hpp"

PF_Err ParameterValueManager::UpdateCurveValues(
    PF_InData* in_data,
    const CurveData* curves,
    A_long curveIndex)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Validate input
    if (!curves || curveIndex < 0 || curveIndex >= 4) {
        return ErrorHandler::getInstance().HandleError(
            in_data,
            nullptr,
            PF_Err_BAD_CALLBACK_PARAM,
            "Invalid curve parameters");
    }

    // Validate curve values
    ERR(ValidateValues(&curves[curveIndex]));
    if (err) return err;

    // Update cache if needed
    while (parameterCache.size() <= static_cast<size_t>(curveIndex)) {
        parameterCache.push_back({false, -1, {}});
    }

    // Update cache entry
    ParameterCache& cache = parameterCache[curveIndex];
    cache.isValid = true;
    cache.curveIndex = curveIndex;
    cache.cachedCurve = curves[curveIndex];

    return err;
}

PF_Err ParameterValueManager::GetCurveValues(
    PF_InData* in_data,
    CurveData* curves,
    A_long curveIndex)
{
    PF_Err err = PF_Err_NONE;

    // Validate input
    if (!curves || curveIndex < 0 || curveIndex >= 4) {
        return ErrorHandler::getInstance().HandleError(
            in_data,
            nullptr,
            PF_Err_BAD_CALLBACK_PARAM,
            "Invalid curve parameters");
    }

    // Check cache
    if (curveIndex < parameterCache.size() && 
        parameterCache[curveIndex].isValid && 
        parameterCache[curveIndex].curveIndex == curveIndex) {
        curves[curveIndex] = parameterCache[curveIndex].cachedCurve;
    } else {
        // Initialize default values if not in cache
        curves[curveIndex].num_points = 2;
        curves[curveIndex].points[0] = {0.0f, 0.0f};
        curves[curveIndex].points[1] = {1.0f, 1.0f};
        curves[curveIndex].dirty = true;
    }

    return err;
}

PF_Err ParameterValueManager::ValidateValues(
    const CurveData* curve)
{
    PF_Err err = PF_Err_NONE;

    if (!curve) return PF_Err_BAD_CALLBACK_PARAM;

    // Validate number of points
    if (curve->num_points < 2 || curve->num_points > 256) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Validate each point
    for (A_long i = 0; i < curve->num_points; i++) {
        ERR(ValidatePoint(curve->points[i]));
        if (err) break;

        // Check x-coordinate ordering
        if (i > 0 && curve->points[i].x <= curve->points[i-1].x) {
            err = PF_Err_BAD_CALLBACK_PARAM;
            break;
        }
    }

    return err;
}

PF_Err ParameterValueManager::ValidatePoint(
    const CurvePoint& point)
{
    // Check point coordinates are within valid range [0,1]
    if (point.x < 0.0f || point.x > 1.0f ||
        point.y < 0.0f || point.y > 1.0f) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    return PF_Err_NONE;
}

PF_Err ParameterValueManager::InterpolateValues(
    const CurveData* curve,
    PF_FpLong* values,
    A_long count)
{
    PF_Err err = PF_Err_NONE;

    // Validate input
    if (!curve || !values || count <= 0) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    // Interpolate values
    for (A_long i = 0; i < count; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (count - 1);
        
        // Find segment containing x
        A_long segIndex = 0;
        for (; segIndex < curve->num_points - 1; segIndex++) {
            if (x <= curve->points[segIndex + 1].x) break;
        }

        // Linear interpolation within segment
        PF_FpLong x0 = curve->points[segIndex].x;
        PF_FpLong y0 = curve->points[segIndex].y;
        PF_FpLong x1 = curve->points[segIndex + 1].x;
        PF_FpLong y1 = curve->points[segIndex + 1].y;

        PF_FpLong t = (x - x0) / (x1 - x0);
        values[i] = y0 + t * (y1 - y0);
    }

    return err;
}