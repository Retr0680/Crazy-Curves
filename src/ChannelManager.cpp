#include "ChannelManager.hpp"
#include "CurvesUI.hpp"
#include "DrawingUtils.hpp"

ChannelManager::ChannelManager() : activeChannel(Channel::RGB) {
    // Initialize all curves
    for (size_t i = 0; i < static_cast<size_t>(Channel::COUNT); ++i) {
        curves[i] = std::make_unique<CurveData>();
    }

    // Initialize selector bounds
    for (int i = 0; i < 4; i++) {
        selectorBounds[i] = {
            .left = 10,
            .top = 10 + (i * 25),
            .right = 60,
            .bottom = 30 + (i * 25)
        };
    }
}

void ChannelManager::setCurrentChannel(Channel channel) {
    activeChannel = channel;
}

Channel ChannelManager::getCurrentChannel() const {
    return activeChannel;
}

CurveData* ChannelManager::getCurve(Channel channel) {
    size_t index = static_cast<size_t>(channel);
    if (index < curves.size()) {
        return curves[index].get();
    }
    return nullptr;
}

const CurveData* ChannelManager::getCurve(Channel channel) const {
    size_t index = static_cast<size_t>(channel);
    if (index < curves.size()) {
        return curves[index].get();
    }
    return nullptr;
}

void ChannelManager::resetChannel(Channel channel) {
    if (auto curve = getCurve(channel)) {
        curve->reset();
    }
}

void ChannelManager::resetAllChannels() {
    for (size_t i = 0; i < static_cast<size_t>(Channel::COUNT); ++i) {
        resetChannel(static_cast<Channel>(i));
    }
}

void ChannelManager::drawChannelSelector(PF_EffectWorld* world, PF_Point origin) {
    const char* labels[] = {"RGB", "R", "G", "B"};
    
    for (int i = 0; i < 4; i++) {
        PF_Rect bounds = selectorBounds[i];
        bounds.left += origin.x;
        bounds.right += origin.x;
        bounds.top += origin.y;
        bounds.bottom += origin.y;
        
        // Draw selector background
        bool isSelected = (activeChannel == static_cast<Channel>(i));
        PF_Pixel color = isSelected ? PF_Pixel{255, 200, 200, 255} : PF_Pixel{180, 180, 180, 255};
        DrawingUtils::drawRect(world, bounds, color);
        
        // Draw label
        DrawingUtils::drawText(world, labels[i], bounds, PF_Pixel{0, 0, 0, 255});
    }
}

bool ChannelManager::handleChannelSelection(PF_Point mousePos) {
    for (int i = 0; i < 4; i++) {
        if (DrawingUtils::pointInRect(mousePos, selectorBounds[i])) {
            setCurrentChannel(static_cast<Channel>(i));
            return true;
        }
    }
    return false;
}