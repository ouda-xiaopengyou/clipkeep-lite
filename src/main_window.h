#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "clip.h"
#include "database.h"
#include <windows.h>
#include <vector>
#include <functional>
#include <atomic>

class MainWindow {
public:
    using PasteHandler = std::function<void(int64_t clip_id)>;
    
    MainWindow();
    ~MainWindow();
    
    bool init(HINSTANCE hInstance);
    void show();
    void hide();
    void toggle();
    
    void set_paste_handler(PasteHandler handler) { paste_handler_ = handler; }
    
    // Database refresh
    void refresh_clips(ClipScope scope, const std::string& query = "");
    
    void set_scope(ClipScope scope) { current_scope_ = scope; }
    ClipScope get_scope() const { return current_scope_; }
    
private:
    HWND hwnd_;
    HINSTANCE hinstance_;
    std::atomic<bool> visible_{false};
    std::atomic<bool> focused_{false};
    
    ClipScope current_scope_ = ClipScope::ALL;
    std::string search_query_;
    PasteHandler paste_handler_;
    
    WNDCLASS wndclass_;
    
    static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
    bool register_window_class(HINSTANCE hInstance);
    HWND create_main_window(HINSTANCE hInstance);
    
    void on_paint();
    void on_size(UINT nState, int cx, int cy);
    void on_setfocus(HWND hwndFocus);
    void on_killfocus(HWND hwndOldFocus);
    
    void draw_list(int hdc, int start_y, int width, int height);
    void draw_item(int hdc, int x, int y, const Clip& clip, bool selected);
    
    void handle_hotkey_trigger();
    void paste_clip_to_previous(Clip& clip);
};

#endif // MAIN_WINDOW_H
