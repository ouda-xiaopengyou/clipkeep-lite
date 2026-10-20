#include "main_window.h"
#include "utilities.h"
#include <windows.h>
#include <commctrl.h>
#include <algorithm>

MainWindow::MainWindow() 
    : hwnd_(nullptr), hinstance_(nullptr) {
}

MainWindow::~MainWindow() {
}

bool MainWindow::register_window_class(HINSTANCE hInstance) {
    wndclass_.cbSize = sizeof(WNDCLASS);
    wndclass_.style = CS_HREDRAW | CS_VREDRAW;
    wndclass_.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
        // Static callback - would dispatch to instance in full impl
        return DefWindowProc(hwnd, msg, wparam, lparam);
    };
    wndclass_.cbClsExtra = 0;
    wndclass_.cbWndExtra = 0;
    wndclass_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass_.hbrBackground = static<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wndclass_.lpszMenuName = nullptr;
    wndclass_.lpszClassName = L"ClipKeepWindow";
    
    if (!RegisterClass(&wndclass_)) {
        return false;
    }
    
    return true;
}

HWND MainWindow::create_main_window(HINSTANCE hInstance) {
    // Create simple undecorated window
    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~WS_MINMAXBUTTON;
    
    RECT rect = {0, 0, 400, 500};
    AdjustWindowRect(&rect, style, FALSE);
    
    hwnd_ = CreateWindowEx(
        0,
        L"ClipKeepWindow",
        L"ClipKeep - Clipboard History",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, nullptr);
    
    if (!hwnd_) {
        return nullptr;
    }
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);
    
    return hwnd_;
}

bool MainWindow::init(HINSTANCE hInstance) {
    hinstance_ = hInstance;
    
    if (!register_window_class(hInstance)) {
        return false;
    }
    
    if (!create_main_window(hInstance)) {
        return false;
    }
    
    return true;
}

void MainWindow::show() {
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
    visible_ = true;
}

void MainWindow::hide() {
    ShowWindow(hwnd_, SW_HIDE);
    visible_ = false;
}

void MainWindow::toggle() {
    if (visible_) {
        hide();
    } else {
        show();
    }
}

void MainWindow::refresh_clips(ClipScope scope, const std::string& query) {
    current_scope_ = scope;
    search_query_ = query;
    
    // In full implementation, would reload list from database
    // and redraw the ListView
}

// Note: This is a minimal stub implementation.
// The full implementation would include:
// - ListView control for displaying clips
// - Keyboard shortcuts (up/down arrows, Enter to paste)
// - Search functionality
// - Context menu with actions (favorite, pin, delete, move to group)
// - Tray icon integration
