#pragma once

#include "AEConfig.h"
#include "entry.h"

enum {
    PARAM_INPUT = 0,
    PARAM_OPACITY,
    PARAM_COUNT
};

enum {
    CURVE_RGB = 0,
    CURVE_RED,
    CURVE_GREEN,
    CURVE_BLUE
};

#ifdef AE_OS_WIN
    #include <Windows.h>
#endif

#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_EffectUI.h"
#include "AEGP_SuiteHandler.h"
#include "String_Utils.h"
#include "Param_Utils.h"

#ifdef AE_OS_WIN
    #define DllExport   __declspec( dllexport )
#else
    #define DllExport   
#endif

// Function declarations
extern "C" {
    DllExport PF_Err EffectMain(
        PF_Cmd          cmd,
        PF_InData       *in_data,
        PF_OutData      *out_data,
        PF_ParamDef     *params[],
        PF_LayerDef     *output,
        void            *extra
    );
}