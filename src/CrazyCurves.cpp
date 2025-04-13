#include "CrazyCurves.h"
#include "CurvesUI.hpp"
#include "CurveCache.hpp"
#include "CurvesData.h"
#include "CurvesUI.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"

// Add these as global variables
static CurvesData g_rgbCurve;
static CurvesData g_redCurve;
static CurvesData g_greenCurve;
static CurvesData g_blueCurve;
static CurvesUI g_curvesUI;

// Add to CrazyCurves.cpp
typedef struct {
    CurvesUI* ui;
    CurveCache* cache;
} SequenceData;

static PF_Err
HandleNewSequenceData(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = new SequenceData();
    
    seqData->ui = new CurvesUI();
    seqData->cache = new CurveCache();
    
    out_data->sequence_data = seqData;
    return err;
}

static PF_Err
HandleDisposeSequenceData(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    delete seqData->ui;
    delete seqData->cache;
    delete seqData;
    
    return err;
}

static PF_Err 
About(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
        "Crazy Curves v1.0\r"
        "Color curve adjustment plugin\r"
        "Copyright 2025");
    return PF_Err_NONE;
}

static PF_Err 
GlobalSetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    out_data->my_version = PF_VERSION(1, 0, 0, 0, 0);
    out_data->out_flags = PF_OutFlag_CUSTOM_UI;
    out_data->out_flags2 = PF_OutFlag2_SUPPORTS_SMART_RENDER;
    return PF_Err_NONE;
}

static PF_Err
ParamsSetup(
    PF_InData		*in_data,
    PF_OutData		*out_data,
    PF_ParamDef		*params[],
    PF_LayerDef		*output)
{
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDER("Master Opacity", 0, 100, 0, 100, 100, 0, 0, 0, 0, PARAM_OPACITY);
    
    out_data->num_params = PARAM_COUNT;
    return err;
}

static PF_Err 
Render(
    PF_InData		*in_data,
    PF_OutData		*out_data,
    PF_ParamDef		*params[],
    PF_LayerDef		*output)
{
    PF_Err err = PF_Err_NONE;
    PF_LayerDef *input_layer = &params[PARAM_INPUT]->u.ld;
    
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // For each pixel in the frame
    for (int y = 0; y < output->height; y++) {
        PF_Pixel* inputP = (PF_Pixel*)((char*)input_layer->data + 
                                      y * input_layer->rowbytes);
        PF_Pixel* outputP = (PF_Pixel*)((char*)output->data + 
                                       y * output->rowbytes);
        
        for (int x = 0; x < output->width; x++) {
            // Apply curves to each channel
            float normalizedR = inputP[x].red / 255.0f;
            float normalizedG = inputP[x].green / 255.0f;
            float normalizedB = inputP[x].blue / 255.0f;
            
            // Apply RGB curve first
            normalizedR = g_rgbCurve.evaluate(normalizedR);
            normalizedG = g_rgbCurve.evaluate(normalizedG);
            normalizedB = g_rgbCurve.evaluate(normalizedB);
            
            // Apply individual channel curves
            normalizedR = g_redCurve.evaluate(normalizedR);
            normalizedG = g_greenCurve.evaluate(normalizedG);
            normalizedB = g_blueCurve.evaluate(normalizedB);
            
            // Convert back to 8-bit color
            outputP[x].red = static_cast<A_u_char>(normalizedR * 255.0f);
            outputP[x].green = static_cast<A_u_char>(normalizedG * 255.0f);
            outputP[x].blue = static_cast<A_u_char>(normalizedB * 255.0f);
            outputP[x].alpha = inputP[x].alpha;
        }
    }
    
    return err;
}

// Add a new function to handle curve UI events
static PF_Err
HandleEvent(
    PF_InData		*in_data,
    PF_OutData		*out_data,
    PF_ParamDef		*params[],
    PF_LayerDef		*output,
    PF_EventExtra	*event_extra)
{
    return g_curvesUI.handleEvent(in_data, out_data, params, output, event_extra);
}

// Update EffectMain to handle UI events
PF_Err
EffectMain(
    PF_Cmd			cmd,
    PF_InData		*in_data,
    PF_OutData		*out_data,
    PF_ParamDef		*params[],
    PF_LayerDef		*output,
    void			*extra)
{
    PF_Err err = PF_Err_NONE;

    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:
                err = About(in_data, out_data, params, output);
                break;
            case PF_Cmd_GLOBAL_SETUP:
                err = GlobalSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_RENDER:
                err = Render(in_data, out_data, params, output);
                break;
            case PF_Cmd_EVENT:
                err = HandleEvent(in_data, out_data, params, output, (PF_EventExtra*)extra);
                break;
            case PF_Cmd_SEQUENCE_SETUP:
                err = HandleNewSequenceData(in_data, out_data);
                break;
            case PF_Cmd_SEQUENCE_SETDOWN:
                err = HandleDisposeSequenceData(in_data, out_data);
                break;
            default:
                break;
        }
    }
    catch(PF_Err &thrown_err) {
        err = thrown_err;
    }

    return err;
}

static PF_Err
SequenceSetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = new SequenceData();
    
    seqData->ui = new CurvesUI();
    seqData->cache = new CurveCache();
    
    out_data->sequence_data = seqData;
    return err;
}

static PF_Err
SequenceSetdown(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    delete seqData->ui;
    delete seqData->cache;
    delete seqData;
    
    return err;
}

DllExport 
PF_Err 
EntryPointFunc(
    PF_Cmd cmd,
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    void* extra)
{
    PF_Err err = PF_Err_NONE;
    
    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:
                err = About(in_data, out_data, params, output);
                break;
            case PF_Cmd_GLOBAL_SETUP:
                err = GlobalSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SEQUENCE_SETUP:
                err = SequenceSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SEQUENCE_SETDOWN:
                err = SequenceSetdown(in_data, out_data, params, output);
                break;
            // Add other command handlers here
        }
    }
    catch(PF_Err &thrown_err) {
        err = thrown_err;
    }
    catch(...) {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    
    return err;
}