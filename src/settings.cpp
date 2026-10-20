#include "settings.h"
#include "utilities.h"
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Settings Settings::load(const std::string& path) {
    Settings settings;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return settings;  // Return defaults
    }
    
    try {
        json j;
        file >> j;
        
        if (j.contains("hotkey_enabled")) {
            settings.hotkey_enabled = j["hotkey_enabled"];
        }
        if (j.contains("hotkey_id")) {
            settings.hotkey_id = j["hotkey_id"];
        }
        if (j.contains("capture_images")) {
            settings.capture_images = j["capture_images"];
        }
        if (j.contains("remember_position")) {
            settings.remember_position = j["remember_position"];
        }
        if (j.contains("window_x") && !j["window_x"].is_null()) {
            settings.window_x = j["window_x"];
        }
        if (j.contains("window_y") && !j["window_y"].is_null()) {
            settings.window_y = j["window_y"];
        }
        if (j.contains("max_storage_mb")) {
            settings.max_storage_mb = j["max_storage_mb"];
        }
        if (j.contains("theme")) {
            int theme_val = j["theme"];
            if (theme_val >= 0 && theme_val <= 2) {
                settings.theme = static_cast<Theme>(theme_val);
            }
        }
    } catch (...) {
        // On error, return defaults
        settings = Settings();
    }
    
    return settings;
}

bool Settings::save(const std::string& path) const {
    json j;
    
    j["hotkey_enabled"] = hotkey_enabled;
    j["hotkey_id"] = hotkey_id;
    j["capture_images"] = capture_images;
    j["remember_position"] = remember_position;
    j["window_x"] = window_x.value_or(null);
    j["window_y"] = window_y.value_or(null);
    j["max_storage_mb"] = max_storage_mb;
    j["theme"] = static_cast<int>(theme);
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(4);
    return true;
}
