#pragma once
#include <map>
#include <functional>

enum class KeyModifiers {
    None = 0,
    Shift = 1,
    Control = 2,
    Alt = 4
};

struct KeyCommand {
    int keyCode;
    KeyModifiers modifiers;
    
    bool operator<(const KeyCommand& other) const {
        if (keyCode != other.keyCode) return keyCode < other.keyCode;
        return static_cast<int>(modifiers) < static_cast<int>(other.modifiers);
    }
};

class KeyboardManager {
public:
    using CommandCallback = std::function<void()>;
    
    void registerCommand(int keyCode, KeyModifiers modifiers, CommandCallback callback);
    bool handleKeyPress(int keyCode, KeyModifiers modifiers);
    void clearCommands();

private:
    std::map<KeyCommand, CommandCallback> commands;
};