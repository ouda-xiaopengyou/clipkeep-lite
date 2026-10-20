#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include "application.h"
#include "utilities.h"

// Check if already running (singleton)
static bool is_another_instance_running() {
    HANDLE mutex = CreateMutexW(nullptr, TRUE, L"ClipKeep_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }
    return false;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, 
                    LPWSTR /*lpCmdLine*/, int nCmdShow) {
    // Handle minimized flag for autostart
    bool minimized = false;
    
    // Check command line args
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 0; i < argc; i++) {
            if (wcscmp(argv[i], L"--minimized") == 0 || 
                wcscmp(argv[i], L"/minimized") == 0) {
                minimized = true;
                break;
            }
        }
        LocalFree(argv);
    }
    
    // Singleton check - show existing instance if running
    if (is_another_instance_running()) {
        // Bring existing window to front would go here
        return 0;
    }
    
    // Initialize application
    Application app;
    
    if (!app.init(hInstance)) {
        MessageBoxW(nullptr, L"Failed to initialize ClipKeep", L"Error", MB_ICONERROR);
        return 1;
    }
    
    // Run message loop
    app.run_message_loop();
    
    return 0;
}

// Console version for debugging (no GUI subsystem)
#ifdef DEBUG_CONSOLE
int main(int argc, char** argv) {
    AllocConsole();
    FILE* console;
    freopen_s(&console, "CONOUT$", "w", stdout);
    freopen_s(&console, "CONIN$", "r", stdin);
    
    std::cout << "ClipKeep Debug Console\n";
    
    // Your debug code here
    
    FreeConsole();
    return 0;
}
#endif
