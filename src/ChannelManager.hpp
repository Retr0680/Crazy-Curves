#pragma once
#include "CurvesUI.hpp"

enum class Channel {
    RGB,
    RED,
    GREEN,
    BLUE
};

class ChannelManager {
public:
    ChannelManager();
    void setCurrentChannel(Channel channel);
    Channel getCurrentChannel() const;
    void drawChannelSelector(PF_EffectWorld* world, PF_Point origin);
    bool handleChannelSelection(PF_Point mousePos);

private:
    Channel currentChannel;
    PF_Rect selectorBounds[4];
};