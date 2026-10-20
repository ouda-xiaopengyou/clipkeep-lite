#include "hotkey_manager.h"
#include <windows.h>

HotkeyManager::HotkeyManager() : hotkey_id_(1) {}

HotkeyManager::~HotkeyManager() {
    uninstall_hotkey();
}

bool HotkeyManager::register_hotkey(HWND hwnd, int id, UINT modifiers, UINT vk) {
    return RegisterHotKey(hwnd, id, modifiers, vk) != 0;
}

void HotkeyManager::unregister_hotkey(HWND hwnd, int id) {
    UnregisterHotKey(hwnd, id);
}

bool HotkeyManager::install_hotkey(const std::string& key, TriggerHandler handler) {
    if (installed_) {
        return false;
    }
    
    // Parse hotkey string (e.g., "win+v", "ctrl+shift+v")
    UINT modifiers = 0;
    UINT vk = 0;
    
    size_t pos = key.find('+');
    while (pos != std::string::npos) {
        std::string mod = key.substr(0, pos);
        transform(mod.begin(), mod.end(), mod.begin(), ::tolower);
        
        if (mod == "win" || mod == "cmd") {
            modifiers |= MOD_WIN;
        } else if (mod == "ctrl" || mod == "control") {
            modifiers |= MOD_CTRL;
        } else if (mod == "alt") {
            modifiers |= MOD_ALT;
        } else if (mod == "shift") {
            modifiers |= MOD_SHIFT;
        }
        
        key = key.substr(pos + 1);
        pos = key.find('+');
    }
    
    // Get last character as virtual key
    if (!key.empty()) {
        char vk_char = tolower(key.back());
        
        if (vk_char >= 'a' && vk_char <= 'z') {
            vk = static_cast<UINT>(VK_A + (vk_char - 'a'));
        } else if (vk_char >= '0' && vk_char <= '9') {
            vk = static_cast<UINT>(VK_0 + (vk_char - '0'));
        } else if (vk_char == '`' || vk_char == '~') {
            vk = VK_OEM_3;
        } else if (vk_char == ',') {
            vk = VK_COMMA;
        } else if (vk_char == '.') {
            vk = VK_DECIMAL;
        } else if (vk_char == '/') {
            vk = VK_DIVIDE;
        } else if (vk == '+') {
            vk = VK_ADD;
        } else if (vk_char == '-') {
            vk = VK_SUBTRACT;
        } else if (vk_char == '*') {
            vk = VK_MULTIPLY;
        } else if (vk_char == 'v') {
            vk = VK_V;
        } else {
            // Unknown key
            return false;
        }
    }
    
    if (vk == 0) {
        return false;
    }
    
    handler_ = handler;
    installed_ = true;
    
    // In a real application, we would store HWND and register here
    // For now, we just mark as installed
    
    return true;
}

void HotkeyManager::uninstall_hotkey() {
    installed_ = false;
    handler_ = nullptr;
    // In real impl: unregister hotkey
}
