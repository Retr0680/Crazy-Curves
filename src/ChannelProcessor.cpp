#include "ChannelProcessor.hpp"
#include "LUTManager.hpp"
#include "SIMDProcessor.hpp"

PF_Err ChannelProcessor::ProcessChannels(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Configure channel processing
    ChannelConfig config;
    config.processRGB = curves[0].num_points > 0;
    config.processRed = curves[1].num_points > 0;
    config.processGreen = curves[2].num_points > 0;
    config.processBlue = curves[3].num_points > 0;

    bool deepColor = PF_WORLD_IS_DEEP(output);
    A_long rowbytes = output->rowbytes;

    // Process pixels
    for (A_long y = 0; y < output->height; y++) {
        if (deepColor) {
            PF_PixelFloat* inP = (PF_PixelFloat*)((char*)input->data + 
                y * input->rowbytes);
            PF_PixelFloat* outP = (PF_PixelFloat*)((char*)output->data + 
                y * rowbytes);

            // Use SIMD for batch processing when possible
            if (y % SIMDProcessor::SIMD_BLOCK_SIZE == 0 && 
                y + SIMDProcessor::SIMD_BLOCK_SIZE <= output->height) {
                ERR(SIMDProcessor::ProcessPixelBlockFloat(
                    inP, outP, curves, SIMDProcessor::SIMD_BLOCK_SIZE));
            } else {
                // Process individual pixels
                for (A_long x = 0; x < output->width; x++) {
                    ERR(ProcessPixelFloatChannels(inP + x, outP + x, curves, config));
                }
            }
        } else {
            PF_Pixel8* inP = (PF_Pixel8*)((char*)input->data + 
                y * input->rowbytes);
            PF_Pixel8* outP = (PF_Pixel8*)((char*)output->data + 
                y * rowbytes);

            // Use SIMD for batch processing when possible
            if (y % SIMDProcessor::SIMD_BLOCK_SIZE == 0 && 
                y + SIMDProcessor::SIMD_BLOCK_SIZE <= output->height) {
                ERR(SIMDProcessor::ProcessPixelBlock8(
                    inP, outP, curves, SIMDProcessor::SIMD_BLOCK_SIZE));
            } else {
                // Process individual pixels
                for (A_long x = 0; x < output->width; x++) {
                    ERR(ProcessPixel8Channels(inP + x, outP + x, curves, config));
                }
            }
        }
    }

    return err;
}

PF_Err ChannelProcessor::ProcessChannel(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curve,
    int channelIndex)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Configure single channel processing
    ChannelConfig config = {false, false, false, false};
    switch (channelIndex) {
        case 0: config.processRGB = true; break;
        case 1: config.processRed = true; break;
        case 2: config.processGreen = true; break;
        case 3: config.processBlue = true; break;
        default: return PF_Err_BAD_CALLBACK_PARAM;
    }

    CurveData curves[4];
    curves[channelIndex] = *curve;

    bool deepColor = PF_WORLD_IS_DEEP(output);
    A_long rowbytes = output->rowbytes;

    // Process single channel
    for (A_long y = 0; y < output->height; y++) {
        if (deepColor) {
            PF_PixelFloat* inP = (PF_PixelFloat*)((char*)input->data + 
                y * input->rowbytes);
            PF_PixelFloat* outP = (PF_PixelFloat*)((char*)output->data + 
                y * rowbytes);

            for (A_long x = 0; x < output->width; x++) {
                ERR(ProcessPixelFloatChannels(inP + x, outP + x, curves, config));
            }
        } else {
            PF_Pixel8* inP = (PF_Pixel8*)((char*)input->data + 
                y * input->rowbytes);
            PF_Pixel8* outP = (PF_Pixel8*)((char*)output->data + 
                y * rowbytes);

            for (A_long x = 0; x < output->width; x++) {
                ERR(ProcessPixel8Channels(inP + x, outP + x, curves, config));
            }
        }
    }

    return err;
}

PF_Err ChannelProcessor::ProcessPixel8Channels(
    const PF_Pixel8* inP,
    PF_Pixel8* outP,
    const CurveData* curves,
    const ChannelConfig& config)
{
    PF_Err err = PF_Err_NONE;
    LUTManager& lutManager = LUTManager::getInstance();

    // Copy input to output
    *outP = *inP;

    // Apply RGB curve first if enabled
    if (config.processRGB) {
        outP->red = static_cast<A_u_char>(lutManager.EvaluateRGB(inP->red / 255.0f) * 255.0f);
        outP->green = static_cast<A_u_char>(lutManager.EvaluateRGB(inP->green / 255.0f) * 255.0f);
        outP->blue = static_cast<A_u_char>(lutManager.EvaluateRGB(inP->blue / 255.0f) * 255.0f);
    }

    // Apply individual channel curves
    if (config.processRed) {
        outP->red = static_cast<A_u_char>(lutManager.EvaluateRed(outP->red / 255.0f) * 255.0f);
    }
    if (config.processGreen) {
        outP->green = static_cast<A_u_char>(lutManager.EvaluateGreen(outP->green / 255.0f) * 255.0f);
    }
    if (config.processBlue) {
        outP->blue = static_cast<A_u_char>(lutManager.EvaluateBlue(outP->blue / 255.0f) * 255.0f);
    }

    return err;
}

PF_Err ChannelProcessor::ProcessPixelFloatChannels(
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP,
    const CurveData* curves,
    const ChannelConfig& config)
{
    PF_Err err = PF_Err_NONE;
    LUTManager& lutManager = LUTManager::getInstance();

    // Copy input to output
    *outP = *inP;

    // Apply RGB curve first if enabled
    if (config.processRGB) {
        outP->red = lutManager.EvaluateRGB(inP->red);
        outP->green = lutManager.EvaluateRGB(inP->green);
        outP->blue = lutManager.EvaluateRGB(inP->blue);
    }

    // Apply individual channel curves
    if (config.processRed) {
        outP->red = lutManager.EvaluateRed(outP->red);
    }
    if (config.processGreen) {
        outP->green = lutManager.EvaluateGreen(outP->green);
    }
    if (config.processBlue) {
        outP->blue = lutManager.EvaluateBlue(outP->blue);
    }

    return err;
}