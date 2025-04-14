#include "ChannelManager.hpp"
#include "CurvesUI.hpp"
#include "DrawingUtils.hpp"
#include "CurveInterpolator.hpp"

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

PF_Err ChannelManager::ProcessChannels(
    const CurveData* curves,
    const PF_Pixel8* inP,
    PF_Pixel8* outP)
{
    PF_Err err = PF_Err_NONE;

    // Apply master RGB curve first
    PF_FpLong rgb_r = ApplyCurve(&curves[0], inP->red / 255.0f, Channel::MASTER);
    PF_FpLong rgb_g = ApplyCurve(&curves[0], inP->green / 255.0f, Channel::MASTER);
    PF_FpLong rgb_b = ApplyCurve(&curves[0], inP->blue / 255.0f, Channel::MASTER);

    // Then apply individual channel curves
    outP->red = static_cast<A_u_char>(ApplyCurve(&curves[1], rgb_r, Channel::RED) * 255.0f);
    outP->green = static_cast<A_u_char>(ApplyCurve(&curves[2], rgb_g, Channel::GREEN) * 255.0f);
    outP->blue = static_cast<A_u_char>(ApplyCurve(&curves[3], rgb_b, Channel::BLUE) * 255.0f);
    outP->alpha = inP->alpha;

    return err;
}

PF_Err ChannelManager::ProcessChannelsFloat(
    const CurveData* curves,
    const PF_PixelFloat* inP,
    PF_PixelFloat* outP)
{
    PF_Err err = PF_Err_NONE;

    // Apply master RGB curve first
    PF_FpLong rgb_r = ApplyCurve(&curves[0], inP->red, Channel::MASTER);
    PF_FpLong rgb_g = ApplyCurve(&curves[0], inP->green, Channel::MASTER);
    PF_FpLong rgb_b = ApplyCurve(&curves[0], inP->blue, Channel::MASTER);

    // Then apply individual channel curves
    outP->red = static_cast<PF_FpShort>(ApplyCurve(&curves[1], rgb_r, Channel::RED));
    outP->green = static_cast<PF_FpShort>(ApplyCurve(&curves[2], rgb_g, Channel::GREEN));
    outP->blue = static_cast<PF_FpShort>(ApplyCurve(&curves[3], rgb_b, Channel::BLUE));
    outP->alpha = inP->alpha;

    return err;
}

PF_FpLong ChannelManager::ApplyCurve(
    const CurveData* curve,
    PF_FpLong input,
    Channel channel)
{
    // Clamp input to [0,1]
    input = input < 0.0f ? 0.0f : (input > 1.0f ? 1.0f : input);

    return CurveInterpolator::InterpolateCurve(curve, input);
}

void ChannelManager::SortChannelPoints(CurveData* curve)
{
    // Simple bubble sort since we typically have few points
    for (A_long i = 0; i < curve->num_points - 1; i++) {
        for (A_long j = 0; j < curve->num_points - i - 1; j++) {
            if (curve->points[j].x > curve->points[j + 1].x) {
                // Swap points
                CurvePoint temp = curve->points[j];
                curve->points[j] = curve->points[j + 1];
                curve->points[j + 1] = temp;
            }
        }
    }
}