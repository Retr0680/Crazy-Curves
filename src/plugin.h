#ifndef PLUGIN_H
#define PLUGIN_H

#include <AEConfig.h>
#include <AE_Effect.h>
#include <AE_EffectCB.h>
#include <AE_Macros.h>
#include <AE_EffectCBSuites.h>

enum {
    CRAZY_CURVES_INPUT = 0,
    CRAZY_CURVES_RED_POINTS,
    CRAZY_CURVES_GREEN_POINTS,
    CRAZY_CURVES_BLUE_POINTS,
    CRAZY_CURVES_MASTER_POINTS,
    CRAZY_CURVES_NUM_PARAMS
};

typedef struct {
    PF_FpLong x;
    PF_FpLong y;
} CurvePoint;

typedef struct {
    CurvePoint points[16];  // Max 16 control points per curve
    int numPoints;
} CurveDef;

typedef struct {
    CurveDef masterCurve;
    CurveDef redCurve;
    CurveDef greenCurve;
    CurveDef blueCurve;
} CrazyCurvesParams;

PF_Err EffectMain(
    PF_Cmd cmd,
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    void* extra);

#endif // PLUGIN_H