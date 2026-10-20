#include "clipboard_monitor.h"
#include <windows.h>
#include <algorithm>

ClipboardMonitor::ClipboardMonitor() {}

ClipboardMonitor::~ClipboardMonitor() {
    stop();
}

void ClipboardMonitor::start(ChangeHandler handler) {
    if (running_.load()) {
        return;
    }
    
    handler_ = handler;
    running_ = true;
    worker_thread_ = std::thread(&ClipboardMonitor::worker_loop, this);
}

void ClipboardMonitor::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_ = false;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void ClipboardMonitor::worker_loop() {
    HANDLE clip_changed_event = CreateEventW(nullptr, TRUE, FALSE, L"ClipboardChanged");
    
    while (running_.load()) {
        // Wait for clipboard change notification
        WaitForSingleObject(clip_changed_event, 100);
        
        if (!running_.load()) {
            break;
        }
        
        // Check if clipboard actually changed
        try_read_and_notify();
    }
    
    CloseHandle(clip_changed_event);
}

bool ClipboardMonitor::try_read_and_notify() {
    // Ensure we have exclusive access to clipboard
    if (!OpenClipboard(nullptr)) {
        return false;
    }
    
    bool has_content = IsClipboardFormatAvailable(CF_UNICODETEXT) != 0 ||
                       IsClipboardFormatAvailable(CF_DIB) != 0;
    
    CloseClipboard();
    
    if (!has_content) {
        return false;
    }
    
    ClipboardData data;
    if (read_clipboard(data)) {
        handler_(data);
    }
    
    return true;
}

bool ClipboardMonitor::read_clipboard(ClipboardData& data) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }
    
    bool success = false;
    
    // Check for text first (text takes precedence over images)
    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        HGLOBAL handle = GetClipboardData(CF_UNICODETEXT);
        if (handle) {
            wchar_t* ptr = static_cast<wchar_t*>(GlobalLock(handle));
            if (ptr) {
                std::wstring wide_text(ptr);
                data.text = wide_to_utf8(wide_text);
                data.type = ClipboardContent::TEXT;
                success = should_record_text(data.text);
                GlobalUnlock(handle);
            }
        }
    } 
    // Check for image (DIB format)
    else if (IsClipboardFormatAvailable(CF_DIB) && capture_images_) {
        HGLOBAL handle = GetClipboardData(CF_DIB);
        if (handle) {
            data.type = ClipboardContent::IMAGE;
            success = true;
            // Image would be saved to file in full implementation
        }
    }
    
    CloseClipboard();
    return success;
}
