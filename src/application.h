#ifndef APPLICATION_H
#define APPLICATION_H

#include "clip.h"
#include <memory>
#include <string>
#include <windows.h>

class Database;
class ClipboardMonitor;
class HotkeyManager;
class MainWindow;
struct Settings;

class Application {
public:
    Application();
    ~Application();
    
    bool init(HINSTANCE hInstance);
    void run_message_loop();
    void exit_app(int exit_code = 0);
    
    static Application& instance();
    
    // Core functionality
    void on_clipboard_change(const ClipboardData& data);
    void paste_clip(int64_t clip_id);
    
    // Getters
    const std::string& data_directory() const { return data_directory_; }
    const std::string& db_file_path() const { return db_file_path_; }
    const std::string& settings_file_path() const { return settings_file_path_; }
    const Settings& settings() const { return settings_; }
    Database* database() { return &db_; }
    
private:
    std::unique_ptr<Database> db_;
    std::unique_ptr<ClipboardMonitor> clipboard_monitor_;
    std::unique_ptr<HotkeyManager> hotkey_manager_;
    std::unique_ptr<MainWindow> main_window_;
    
    std::string data_directory_;
    std::string db_file_path_;
    std::string settings_file_path_;
    Settings settings_;
    
    LRESULT CALLBACK process_window_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
    void enforce_storage_limit();
    void refresh_clips_list();
};

#endif // APPLICATION_H
