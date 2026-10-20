#ifndef CLIP_H
#define CLIP_H

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

enum class ClipType {
    TEXT,
    IMAGE
};

struct Clip {
    int64_t id;
    ClipType type;
    std::string preview;      // First 400 chars of text or empty for images
    int64_t size;             // Size in bytes
    bool truncated;           // Whether text was truncated for preview
    uint32_t width;           // Image width (0 for text)
    uint32_t height;          // Image height (0 for text)
    bool favorite;
    bool pinned;
    int64_t updated_at;       // Timestamp in milliseconds
    std::optional<std::string> source;
    std::optional<int64_t> group_id;
    std::string file;         // Image filename (empty for text)
    std::string thumb;        // Thumbnail path
    
    Clip() : id(0), type(ClipType::TEXT), size(0), truncated(false),
             width(0), height(0), favorite(false), pinned(false),
             updated_at(0) {}
};

struct Group {
    int64_t id;
    std::string name;
    int64_t sort_order;
    
    Group() : id(0), sort_order(0) {}
    Group(int64_t i, const std::string& n, int64_t s) 
        : id(i), name(n), sort_order(s) {}
};

struct RawClip {
    std::string kind;
    std::string content;
    std::string hash;
    std::optional<int64_t> width;
    std::optional<int64_t> height;
    int64_t size;
    bool favorite;
    bool pinned;
    std::optional<int64_t> group_id;
    int64_t created_at;
    int64_t updated_at;
};

struct StorageUsage {
    int64_t ordinary_bytes;
    int64_t protected_bytes;
};

const char* KIND_TEXT = "text";
const char* KIND_IMAGE = "image";
const int PREVIEW_CHARS = 400;
const int MAX_TEXT_BYTES = 2 * 1024 * 1024;
const int64_t MAX_PIXELS = 40'000'000;  // 40 megapixels max
const int64_t DEFAULT_MAX_STORAGE = 500LL * 1024 * 1024;  // 500MB

#endif // CLIP_H
