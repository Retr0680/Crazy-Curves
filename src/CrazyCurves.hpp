#pragma once
#include "AEConfig.h"
#include "entry.h"
#include "AEGP_SuiteHandler.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"

enum {
    CURVEDATA_DISK_ID = 1,
    CURVEDATA_VERSION = 1
};

typedef struct {
    A_long curve_data_id;
    PF_FpLong points[256][2];  // x,y pairs
    A_long num_points;
} CurveData;

typedef struct {
    CurveData rgb_curve;
    CurveData r_curve;
    CurveData g_curve;
    CurveData b_curve;
    PF_EffectWorld *input_worldP;
    PF_Rect preview_rect;
} GlobalData;

PF_Err EffectMain(
    PF_Cmd cmd,
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output,
    void *extra);