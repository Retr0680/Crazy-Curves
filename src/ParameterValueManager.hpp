#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class ParameterValueManager {
public:
    static ParameterValueManager& getInstance() {
        static ParameterValueManager instance;
        return instance;
    }

    PF_Err UpdateCurveValues(
        PF_InData* in_data,
        const CurveData* curves,
        A_long curveIndex);

    PF_Err GetCurveValues(
        PF_InData* in_data,
        CurveData* curves,
        A_long curveIndex);

    PF_Err ValidateValues(
        const CurveData* curve);

private:
    struct ParameterCache {
        bool isValid;
        A_long curveIndex;
        CurveData cachedCurve;
    };

    std::vector<ParameterCache> parameterCache;
    
    PF_Err InterpolateValues(
        const CurveData* curve,
        PF_FpLong* values,
        A_long count);

    PF_Err ValidatePoint(
        const CurvePoint& point);
};