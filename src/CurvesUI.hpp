#pragma once

#include <map>
#include <functional>
#include "ChannelManager.h"
#include "PreviewWindow.h"
#include "CurveHistory.h"
#include "PF_EventExtra.h"

class CurvesUI {
public:
    CurvesUI();
    ~CurvesUI();

    void render();
    void update();

private:
    ChannelManager channelManager;
    PreviewWindow preview;
    CurveHistory history;
    
    struct KeyboardShortcut {
        uint8_t key;
        bool ctrl;
        bool shift;
        bool alt;
        
        bool operator<(const KeyboardShortcut& other) const;
    };
    
    std::map<KeyboardShortcut, std::function<void()>> shortcuts;
    void initializeShortcuts();
    void handleKeyboard(const PF_EventExtra* event);
};