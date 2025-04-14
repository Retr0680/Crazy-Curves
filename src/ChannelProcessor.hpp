#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ChannelProcessor {
public:
    static ChannelProcessor& getInstance() {
        static ChannelProcessor instance;
        return instance;
    }

    PF_Err ProcessChannels(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);

    PF_Err ProcessChannel(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curve,
        int channelIndex);

private:
    struct ChannelConfig {
        bool processRGB;
        bool processRed;
        bool processGreen;
        bool processBlue;
    };

    PF_Err ProcessPixel8Channels(
        const PF_Pixel8* inP,
        PF_Pixel8* outP,
        const CurveData* curves,
        const ChannelConfig& config);

    PF_Err ProcessPixelFloatChannels(
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP,
        const CurveData* curves,
        const ChannelConfig& config);
};