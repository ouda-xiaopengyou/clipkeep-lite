#ifndef CLIPBOARD_MONITOR_H
#define CLIPBOARD_MONITOR_H

#include <functional>
#include <thread>
#include <atomic>
#include <chrono>

enum class ClipboardContent {
    TEXT,
    IMAGE
};

struct ClipboardData {
    ClipboardContent type;
    std::string text;
    // Image would be stored as path to file
};

class ClipboardMonitor {
public:
    using ChangeHandler = std::function<void(const ClipboardData&)>;
    
    ClipboardMonitor();
    ~ClipboardMonitor();
    
    void start(ChangeHandler handler);
    void stop();
    
    bool is_running() const { return running_.load(); }
    
    void set_capture_paused(bool paused) { capture_paused_ = paused; }
    bool capture_paused() const { return capture_paused_; }
    
private:
    std::thread worker_thread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> capture_paused_{false};
    ChangeHandler handler_;
    
    void worker_loop();
    bool read_clipboard(ClipboardData& data);
};

#endif // CLIPBOARD_MONITOR_H
