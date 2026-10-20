#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H

#include <functional>
#include <atomic>
#include <string>

class HotkeyManager {
public:
    using TriggerHandler = std::function<void()>;
    
    HotkeyManager();
    ~HotkeyManager();
    
    bool install_hotkey(const std::string& key, TriggerHandler handler);
    void uninstall_hotkey();
    
    bool is_installed() const { return installed_; }
    
private:
    std::atomic<bool> installed_{false};
    TriggerHandler handler_;
    UINT hotkey_id_;
    
    static bool register_hotkey(HWND hwnd, int id, UINT modifiers, UINT vk);
    static void unregister_hotkey(HWND hwnd, int id);
};

// Hotkey modifiers
enum HotkeyModifiers : DWORD {
    MOD_ALT = 1,
    MOD_CTRL = 2,
    MOD_SHIFT = 4,
    MOD_WIN = 8
};

#endif // HOTKEY_MANAGER_H
