#pragma once

#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_EffectUI.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "String_Utils.h"
#include "Param_Utils.h"

// Plugin parameters
enum {
    PARAM_INPUT = 0,
    PARAM_CURVE_DATA,
    PARAM_COUNT
};

// Curve data structure
typedef struct {
    A_long curve_data_id;
    PF_FpLong points[256][2];  // x,y pairs
    A_long num_points;
} CurveData;

// Sequence data structure
typedef struct {
    CurveData rgb_curve;
    CurveData r_curve;
    CurveData g_curve;
    CurveData b_curve;
    PF_EffectWorld *input_worldP;
} SequenceData;

#ifdef AE_OS_WIN
    #define DllExport __declspec(dllexport)
#else
    #define DllExport
#endif

extern "C" {
    DllExport PF_Err EffectMain(
        PF_Cmd cmd,
        PF_InData *in_data,
        PF_OutData *out_data,
        PF_ParamDef *params[],
        PF_LayerDef *output,
        void *extra);
}