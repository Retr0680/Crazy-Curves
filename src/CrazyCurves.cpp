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
    
    if (!seqData) return PF_Err_OUT_OF_MEMORY;
    
    seqData->ui = new CurvesUI();
    seqData->cache = new CurveCache();
    
    if (!seqData->ui || !seqData->cache) {
        delete seqData->ui;
        delete seqData->cache;
        delete seqData;
        return PF_Err_OUT_OF_MEMORY;
    }
    
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
    out_data->out_flags = PF_OutFlag_CUSTOM_UI | PF_OutFlag_DEEP_COLOR_AWARE;
    out_data->out_flags2 = PF_OutFlag2_SUPPORTS_SMART_RENDER | PF_OutFlag2_FLOAT_COLOR_AWARE;
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
ProcessPixels(
    void* refcon,
    A_long xL, 
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply curve adjustments
    outP->alpha = inP->alpha;
    outP->red   = seqData->cache->getCachedValue(inP->red / 255.0f) * 255;
    outP->green = seqData->cache->getCachedValue(inP->green / 255.0f) * 255;
    outP->blue  = seqData->cache->getCachedValue(inP->blue / 255.0f) * 255;
    
    return PF_Err_NONE;
}

static PF_Err
ProcessPixelsFloat(
    void* refcon,
    A_long xL, 
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply curve adjustments
    outP->alpha = inP->alpha;
    outP->red   = seqData->cache->getCachedValue(inP->red);
    outP->green = seqData->cache->getCachedValue(inP->green);
    outP->blue  = seqData->cache->getCachedValue(inP->blue);
    
    return PF_Err_NONE;
}

static PF_Err 
Render(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    
    // Check if we're in 16/32-bit mode
    bool deepColor = (PF_WORLD_IS_DEEP(output));
    
    PF_EffectWorld* input_worldP = &params[PARAM_INPUT]->u.ld;
    PF_EffectWorld* output_worldP = output;
    
    // Iterate through pixels using AE's iterator
    PF_ParamDef param_copy;
    AEFX_CLR_STRUCT(param_copy);
    
    ERR(suites.ParamUtilsSuite3()->PF_GetCurrentState(params[PARAM_INPUT], in_data->current_time, in_data->time_step, in_data->time_scale, &param_copy));
    
    if (!err) {
        if (deepColor) {
            ERR(suites.Iterate16Suite1()->iterate(in_data,
                                                0,                // progress base
                                                output_worldP->height,    // progress final
                                                input_worldP,     // src
                                                nullptr,         // area - null for all pixels
                                                (void*)seqData,  // refcon
                                                ProcessPixelsFloat,
                                                output_worldP));
        } else {
            ERR(suites.Iterate8Suite1()->iterate(in_data,
                                               0,                // progress base
                                               output_worldP->height,    // progress final
                                               input_worldP,     // src
                                               nullptr,         // area - null for all pixels
                                               (void*)seqData,  // refcon
                                               ProcessPixels,
                                               output_worldP));
        }
    }
    
    return err;
}

// Add a new function to handle curve UI events
static PF_Err
HandleEvent(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    PF_EventExtra* event)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    switch (event->e_type) {
        case PF_Event_DRAW:
            if (seqData && seqData->ui) {
                err = seqData->ui->drawCurve(in_data, out_data, params, event->u.draw.world);
            }
            break;
            
        case PF_Event_DO_CLICK:
            if (seqData && seqData->ui) {
                PF_Point clickPt = {event->u.do_click.screen_point.h, 
                                  event->u.do_click.screen_point.v};
                err = seqData->ui->handleClick(clickPt);
                if (!err) {
                    // Update cache after UI interaction
                    seqData->cache->invalidate();
                    out_data->refresh_flag = true;
                }
            }
            break;
            
        case PF_Event_KEYDOWN:
            if (seqData && seqData->ui) {
                err = seqData->ui->handleKeyboard(event);
                if (!err && (event->u.key.key_code == PF_KEY_DELETE || 
                           event->u.key.key_code == PF_KEY_BACK)) {
                    seqData->cache->invalidate();
                    out_data->refresh_flag = true;
                }
            }
            break;
    }
    
    return err;
}

// Add SmartPreRender function
static PF_Err
SmartPreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->output_request;
    
    // Request full-sized output buffer
    req.preserve_rgb_of_zero_alpha = true;
    
    ERR(extra->cb->checkout_layer(in_data->effect_ref,
                                PARAM_INPUT,
                                PARAM_INPUT,
                                &req,
                                in_data->current_time,
                                in_data->time_step,
                                in_data->time_scale,
                                &extra->input->pre_render_data));
    
    return err;
}

// Add SmartRender function
static PF_Err
SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra)
{
    PF_Err err = PF_Err_NONE;
    SequenceData* seqData = static_cast<SequenceData*>(in_data->sequence_data);
    
    if (!seqData || !seqData->cache) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    
    PF_EffectWorld* input_worldP = nullptr;
    PF_EffectWorld* output_worldP = nullptr;
    
    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, PARAM_INPUT, &input_worldP));
    ERR(extra->cb->get_output_world_ptr(in_data->effect_ref, &output_worldP));
    
    if (!err) {
        AEGP_SuiteHandler suites(in_data->pica_basicP);
        bool deepColor = (PF_WORLD_IS_DEEP(output_worldP));
        
        if (deepColor) {
            ERR(suites.Iterate16Suite1()->iterate_origin(in_data,
                                                      0,
                                                      output_worldP->height,
                                                      input_worldP,
                                                      nullptr,
                                                      (void*)seqData,
                                                      ProcessPixelsFloat,
                                                      output_worldP));
        } else {
            ERR(suites.Iterate8Suite1()->iterate_origin(in_data,
                                                     0,
                                                     output_worldP->height,
                                                     input_worldP,
                                                     nullptr,
                                                     (void*)seqData,
                                                     ProcessPixels,
                                                     output_worldP));
        }
    }
    
    return err;
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
            case PF_Cmd_SMART_PRE_RENDER:
                err = SmartPreRender(in_data, out_data, (PF_PreRenderExtra*)extra);
                break;
            case PF_Cmd_SMART_RENDER:
                err = SmartRender(in_data, out_data, (PF_SmartRenderExtra*)extra);
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
    
    if (!seqData) return PF_Err_OUT_OF_MEMORY;
    
    seqData->ui = new CurvesUI();
    seqData->cache = new CurveCache();
    
    if (!seqData->ui || !seqData->cache) {
        delete seqData->ui;
        delete seqData->cache;
        delete seqData;
        return PF_Err_OUT_OF_MEMORY;
    }
    
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
            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SEQUENCE_SETUP:
                err = SequenceSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SEQUENCE_SETDOWN:
                err = SequenceSetdown(in_data, out_data, params, output);
                break;
            case PF_Cmd_RENDER:
                err = Render(in_data, out_data, params, output);
                break;
            case PF_Cmd_EVENT:
                err = HandleEvent(in_data, out_data, params, output, (PF_EventExtra*)extra);
                break;
            case PF_Cmd_SMART_PRE_RENDER:
                err = SmartPreRender(in_data, out_data, (PF_PreRenderExtra*)extra);
                break;
            case PF_Cmd_SMART_RENDER:
                err = SmartRender(in_data, out_data, (PF_SmartRenderExtra*)extra);
                break;
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