#pragma once
#include <array>
#include <memory>
#include "CurveData.hpp"

class ChannelManager {
public:
    enum class Channel {
        RGB = 0,
        RED,
        GREEN,
        BLUE,
        ALPHA,
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

private:
    std::array<std::unique_ptr<CurveData>, static_cast<size_t>(Channel::COUNT)> curves;
    Channel activeChannel;
};