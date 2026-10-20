#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

namespace fs = std::filesystem;

// Get application data directory
inline std::string get_app_data_dir() {
    auto home = std::getenv("APPDATA");
    if (home && *home) {
        return std::string(home) + "\\ClipKeep";
    }
    return "";
}

// Get current timestamp in milliseconds
inline int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Convert wide string to narrow
inline std::wstring utf8_to_wide(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring ret(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &ret[0], size_needed);
    return ret;
}

// Convert narrow string to wide
inline std::string wide_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string ret(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &ret[0], size_needed, NULL, NULL);
    return ret;
}

// Generate hash from text (simplified BLAKE3 alternative using standard library)
inline std::string text_hash(const std::string& text) {
    std::hash<std::string> hasher;
    auto h = hasher(text);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << h;
    // Use first 32 chars as simplified hash representation
    return oss.str();
}

// Check if text should be recorded
inline bool should_record_text(const std::string& text) {
    if (text.empty() || text.length() > 2 * 1024 * 1024) {
        return false;
    }
    // Exclude LAN pairing codes
    if (text.find("cklan1://") != std::string::npos) {
        return false;
    }
    return !text.find_first_not_of(" \t\r\n") == std::string::npos;
}

// Trim whitespace
inline std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

#endif // UTILITIES_H
