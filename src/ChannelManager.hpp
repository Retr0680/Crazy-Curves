#pragma once
#include <array>
#include <memory>
#include "CurveData.hpp"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"

class ChannelManager {
public:
    enum class Channel {
        MASTER = 0,
        RED,
        GREEN, 
        BLUE,
        COUNT
    };

    ChannelManager();
    ~ChannelManager() = default;

    void setActiveChannel(Channel channel) { activeChannel = channel; }
    Channel getActiveChannel() const { return activeChannel; }
    
    CurveData* getCurve(Channel channel);
    const CurveData* getCurve(Channel channel) const;
    
    void resetChannel(Channel channel);
    void resetAllChannels();

    static PF_Err ProcessChannels(
        const CurveData* curves,
        const PF_Pixel8* inP,
        PF_Pixel8* outP);

    static PF_Err ProcessChannelsFloat(
        const CurveData* curves,
        const PF_PixelFloat* inP,
        PF_PixelFloat* outP);

private:
    std::array<std::unique_ptr<CurveData>, static_cast<size_t>(Channel::COUNT)> curves;
    Channel activeChannel;

    static PF_FpLong ApplyCurve(
        const CurveData* curve,
        PF_FpLong input,
        Channel channel);

    static void SortChannelPoints(CurveData* curve);
};