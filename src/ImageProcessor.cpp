#pragma once
#include "ImageProcessor.hpp"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include <algorithm>

ImageProcessor::ImageProcessor() : channelManager(nullptr) {}

void ImageProcessor::processImage(uint8_t* data, int width, int height, int stride) {
    if (!channelManager) return;
    
    updateCaches();
    
    for (int y = 0; y < height; ++y) {
        uint8_t* row = data + y * stride;
        for (int x = 0; x < width; ++x) {
            processPixel(row + x * 4);
        }
    }
}

void ImageProcessor::updateCaches() {
    for (size_t i = 0; i < caches.size(); ++i) {
        auto channel = static_cast<ChannelManager::Channel>(i);
        if (auto curve = channelManager->getCurve(channel)) {
            caches[i].updateCache(*curve);
        }
    }
}

void ImageProcessor::processPixel(uint8_t* pixel) {
    // Process RGB channels
    for (int i = 0; i < 3; ++i) {
        float value = pixel[i] / 255.0f;
        
        // Apply RGB curve first
        value = processChannel(value * 255.0f, ChannelManager::Channel::RGB) / 255.0f;
        
        // Then apply individual channel curve
        auto channel = static_cast<ChannelManager::Channel>(i + 1); // RED = 1, GREEN = 2, BLUE = 3
        value = processChannel(value * 255.0f, channel) / 255.0f;
        
        pixel[i] = static_cast<uint8_t>(std::clamp(value * 255.0f, 0.0f, 255.0f));
    }
    
    // Process Alpha channel separately
    float alpha = pixel[3] / 255.0f;
    alpha = processChannel(alpha * 255.0f, ChannelManager::Channel::ALPHA) / 255.0f;
    pixel[3] = static_cast<uint8_t>(std::clamp(alpha * 255.0f, 0.0f, 255.0f));
}

float ImageProcessor::processChannel(uint8_t value, ChannelManager::Channel channel) {
    return caches[static_cast<size_t>(channel)].getCachedValue(value / 255.0f) * 255.0f;
}

PF_Err ProcessImageWorld(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    SequenceData* seqData,
    const AEGP_SuiteHandler& suites)
{
    PF_Err err = PF_Err_NONE;
    
    // Determine color depth
    bool deepColor = PF_WORLD_IS_DEEP(output);
    
    if (deepColor) {
        ERR(suites.Iterate16Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            &input->extent_hint,        // area to process
            seqData,                    // refcon
            ProcessPixelFloat,          // pixel function pointer
            output));                   // dest
    } else {
        ERR(suites.Iterate8Suite1()->iterate_origin(
            in_data,
            0,                          // progress base
            output->height,             // progress final
            input,                      // src
            &input->extent_hint,        // area to process
            seqData,                    // refcon
            ProcessPixel8,              // pixel function pointer
            output));                   // dest
    }
    
    return err;
}

static PF_Err 
ProcessPixel8(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red / 255.0f);
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green / 255.0f);
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue / 255.0f);
    
    // Then apply individual channel curves
    outP->red   = static_cast<A_u_char>(CurvesData::evaluate(&seqData->r_curve, rgb_r) * 255.0f);
    outP->green = static_cast<A_u_char>(CurvesData::evaluate(&seqData->g_curve, rgb_g) * 255.0f);
    outP->blue  = static_cast<A_u_char>(CurvesData::evaluate(&seqData->b_curve, rgb_b) * 255.0f);
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}

static PF_Err 
ProcessPixelFloat(
    void* refcon,
    A_long xL,
    A_long yL,
    PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    SequenceData* seqData = static_cast<SequenceData*>(refcon);
    
    // Apply RGB curve first
    PF_FpLong rgb_r = CurvesData::evaluate(&seqData->rgb_curve, inP->red);
    PF_FpLong rgb_g = CurvesData::evaluate(&seqData->rgb_curve, inP->green);
    PF_FpLong rgb_b = CurvesData::evaluate(&seqData->rgb_curve, inP->blue);
    
    // Then apply individual channel curves
    outP->red   = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->r_curve, rgb_r));
    outP->green = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->g_curve, rgb_g));
    outP->blue  = static_cast<PF_FpShort>(CurvesData::evaluate(&seqData->b_curve, rgb_b));
    outP->alpha = inP->alpha;
    
    return PF_Err_NONE;
}