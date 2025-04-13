#include "KeyboardManager.hpp"

KeyboardManager::KeyboardManager() {
    // Initialize with default shortcuts
    registerCommand('Z', true, false, false, []() { /* Undo */ });
    registerCommand('Z', true, true, false, []() { /* Redo */ });
    registerCommand('C', true, false, false, []() { /* Copy */ });
    registerCommand('V', true, false, false, []() { /* Paste */ });
    registerCommand(PF_KEY_DELETE, false, false, false, []() { /* Delete point */ });
}

void KeyboardManager::registerCommand(uint8_t key, bool ctrl, bool shift, bool alt, 
                                   std::function<void()> callback) {
    KeyCommand cmd{key, ctrl, shift, alt};
    commands[cmd] = callback;
}

bool KeyboardManager::handleKeyEvent(const PF_EventExtra* event) {
    if (event->e_type != PF_Event_KEYDOWN) {
        return false;
    }

    KeyCommand cmd{
        event->u.key.key_code,
        (event->u.key.modifier_flags & PF_Mod_COMMAND) != 0,
        (event->u.key.modifier_flags & PF_Mod_SHIFT) != 0,
        (event->u.key.modifier_flags & PF_Mod_ALT) != 0
    };

    auto it = commands.find(cmd);
    if (it != commands.end()) {
        it->second();
        return true;
    }

    return false;
}