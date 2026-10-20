#include "application.h"
#include "database.h"
#include "clipboard_monitor.h"
#include "hotkey_manager.h"
#include "main_window.h"
#include "settings.h"
#include "utilities.h"
#include <windows.h>
#include <thread>

static Application* g_app = nullptr;

Application::Application() {
    g_app = this;
}

Application::~Application() {
    g_app = nullptr;
}

bool Application::init(HINSTANCE hInstance) {
    // Create app data directory
    std::string data_dir = get_app_data_dir();
    fs::create_directories(data_dir);
    
    data_directory_ = data_dir;
    db_file_path_ = data_dir + "\\clipkeep.db";
    settings_file_path_ = data_dir + "\\settings.json";
    
    // Load settings
    settings_ = Settings::load(settings_file_path_);
    
    // Open database
    db_ = std::make_unique<Database>();
    if (!db_->open(db_file_path_)) {
        return false;
    }
    
    // Initialize clipboard monitor
    clipboard_monitor_ = std::make_unique<ClipboardMonitor>();
    clipboard_monitor_->start([this](const ClipboardData& data) {
        on_clipboard_change(data);
    });
    
    // Initialize hotkey manager
    hotkey_manager_ = std::make_unique<HotkeyManager>();
    hotkey_manager_->install_hotkey("win+v", [this] {
        main_window_->toggle();
    });
    
    // Initialize main window
    main_window_ = std::make_unique<MainWindow>();
    main_window_->set_paste_handler([this](int64_t clip_id) {
        paste_clip(clip_id);
    });
    
    // Show window
    main_window_->show();
    
    return true;
}

void Application::on_clipboard_change(const ClipboardData& data) {
    if (clipboard_monitor_->capture_paused()) {
        return;
    }
    
    if (data.type == ClipboardContent::TEXT && should_record_text(data.text)) {
        std::string hash = text_hash(data.text);
        
        // Check for duplicates using hash
        auto existing = db_->find_by_hash(hash);
        if (existing.has_value()) {
            // Update timestamp
            db_->touch(existing.value());
            return;
        }
        
        // Insert new clip
        int64_t id = db_->insert_text(
            data.text, 
            hash, 
            static_cast<int64_t>(data.text.length())
        );
        
        if (id > 0) {
            enforce_storage_limit();
        }
    }
}

void Application::paste_clip(int64_t clip_id) {
    auto clip_opt = db_->get_text(clip_id);
    if (!clip_opt.has_value()) {
        return;
    }
    
    const std::string& content = clip_opt.value();
    
    // Write to clipboard
    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        
        // Convert UTF-8 to UTF-16
        std::wstring wide_content = utf8_to_wide(content);
        size_t size_needed = (wide_content.length() + 1) * sizeof(wchar_t);
        
        HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE, size_needed);
        if (hglob) {
            wchar_t* ptr = static_cast<wchar_t*>(GlobalLock(hglob));
            if (ptr) {
                memcpy(ptr, wide_content.c_str(), size_needed);
                GlobalUnlock(hglob);
                SetClipboardData(CF_UNICODETEXT, hglob);
                
                // Touch the clip
                db_->touch(clip_id);
            } else {
                GlobalFree(hglob);
            }
        }
        
        CloseClipboard();
    }
}

void Application::enforce_storage_limit() {
    int64_t max_bytes = settings_.max_storage_bytes();
    if (max_bytes <= 0) {
        return;
    }
    
    StorageUsage usage = db_->get_storage_usage();
    if (usage.ordinary_bytes > max_bytes) {
        db_->enforce_storage_limit(max_bytes);
    }
}

void Application::run_message_loop() {
    MSG msg;
    
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
