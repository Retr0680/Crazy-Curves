#include "ColorProcessor.hpp"

PF_Err ColorProcessor::ProcessFrame(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Initialize LUT Manager with current curves
    LUTManager& lutManager = LUTManager::getInstance();
    ERR(lutManager.InitializeLUTs(curves));

    if (!err) {
        // Process based on color depth
        if (PF_WORLD_IS_DEEP(output)) {
            ERR(ProcessPixelsFloat(input, output, lutManager));
        } else {
            ERR(ProcessPixels8(input, output, lutManager));
        }
    }

    return err;
}

PF_Err ColorProcessor::ProcessPixels8(
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const LUTManager& lutManager)
{
    PF_Err err = PF_Err_NONE;
    
    for (A_long y = 0; y < output->height; y++) {
        PF_Pixel8* inP = (PF_Pixel8*)((char*)input->data + y * input->rowbytes);
        PF_Pixel8* outP = (PF_Pixel8*)((char*)output->data + y * output->rowbytes);
        
        for (A_long x = 0; x < output->width; x++) {
            ERR(lutManager.ProcessPixel8(inP++, outP++));
            if (err) break;
        }
        if (err) break;
    }
    
    return err;
}

PF_Err ColorProcessor::ProcessPixelsFloat(
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const LUTManager& lutManager)
{
    PF_Err err = PF_Err_NONE;
    
    for (A_long y = 0; y < output->height; y++) {
        PF_PixelFloat* inP = (PF_PixelFloat*)((char*)input->data + y * input->rowbytes);
        PF_PixelFloat* outP = (PF_PixelFloat*)((char*)output->data + y * output->rowbytes);
        
        for (A_long x = 0; x < output->width; x++) {
            ERR(lutManager.ProcessPixelFloat(inP++, outP++));
            if (err) break;
        }
        if (err) break;
    }
    
    return err;
}