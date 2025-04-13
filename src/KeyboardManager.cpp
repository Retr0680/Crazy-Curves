#include "KeyboardManager.hpp"

KeyboardManager::KeyboardManager() {
    // Initialize with default shortcuts
    registerCommand('Z', {true, false, false}, []() { /* Undo */ });
    registerCommand('Z', {true, true, false}, []() { /* Redo */ });
    registerCommand('C', {true, false, false}, []() { /* Copy */ });
    registerCommand('V', {true, false, false}, []() { /* Paste */ });
    registerCommand(PF_KEY_DELETE, {false, false, false}, []() { /* Delete point */ });
}

void KeyboardManager::registerCommand(int keyCode, KeyModifiers modifiers, CommandCallback callback) {
    KeyCommand cmd{keyCode, modifiers};
    commands[cmd] = callback;
}

bool KeyboardManager::handleKeyPress(int keyCode, KeyModifiers modifiers) {
    KeyCommand cmd{keyCode, modifiers};
    auto it = commands.find(cmd);
    
    if (it != commands.end()) {
        it->second();
        return true;
    }
    
    return false;
}

void KeyboardManager::clearCommands() {
    commands.clear();
}