#pragma once
#include <map>
#include <functional>
#include "AE_Effect.h"

struct KeyCommand {
    uint8_t key;
    bool ctrl;
    bool shift;
    bool alt;

    bool operator<(const KeyCommand& other) const {
        if (key != other.key) return key < other.key;
        if (ctrl != other.ctrl) return ctrl < other.ctrl;
        if (shift != other.shift) return shift < other.shift;
        return alt < other.alt;
    }
};

class KeyboardManager {
public:
    KeyboardManager();
    void registerCommand(uint8_t key, bool ctrl, bool shift, bool alt, std::function<void()> callback);
    bool handleKeyEvent(const PF_EventExtra* event);

private:
    std::map<KeyCommand, std::function<void()>> commands;
};