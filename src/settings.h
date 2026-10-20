#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <optional>

enum class Theme {
    SYSTEM,
    LIGHT,
    DARK
};

struct Settings {
    // Hotkey settings
    bool hotkey_enabled = true;
    std::string hotkey_id = "win+v";  // win+v, ctrl+shift+v, etc.
    
    // Clipboard capture
    bool capture_images = true;
    
    // Window settings
    bool remember_position = false;
    std::optional<int32_t> window_x;
    std::optional<int32_t> window_y;
    
    // Storage settings
    int64_t max_storage_mb = 500;
    
    // Appearance
    Theme theme = Theme::SYSTEM;
    
    // Save/Load from file
    static Settings load(const std::string& path);
    bool save(const std::string& path) const;
    
    int64_t max_storage_bytes() const {
        return static_cast<int64_t>(max_storage_mb) * 1024 * 1024;
    }
};

#endif // SETTINGS_H
