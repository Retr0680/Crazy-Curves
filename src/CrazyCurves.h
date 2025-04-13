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

// Version info
#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUG_VERSION  0
#define STAGE_VERSION PF_Stage_DEVELOP
#define BUILD_VERSION 1

// Plugin parameters
enum {
    PARAM_INPUT = 0,
    PARAM_CURVES_GROUP,
    PARAM_RGB_CURVE,
    PARAM_R_CURVE,
    PARAM_G_CURVE,
    PARAM_B_CURVE,
    PARAM_OPACITY,
    PARAM_COUNT
};

// Curve data structure
typedef struct {
    PF_FpLong x;
    PF_FpLong y;
} CurvePoint;

typedef struct {
    A_long curve_id;
    CurvePoint points[256];
    A_long num_points;
    PF_Boolean dirty;
} CurveData;

// Sequence data structure
typedef struct {
    CurveData rgb_curve;
    CurveData r_curve;
    CurveData g_curve;
    CurveData b_curve;
    A_long selected_point;
    PF_Boolean needs_update;
    void* cache;
} SequenceData;

#ifdef AE_OS_WIN
    #define DllExport __declspec(dllexport)
#else
    #define DllExport
#endif

extern "C" {
    DllExport PF_Err EntryPointFunc(
        PF_Cmd cmd,
        PF_InData *in_data,
        PF_OutData *out_data,
        PF_ParamDef *params[],
        PF_LayerDef *output,
        void *extra);
}