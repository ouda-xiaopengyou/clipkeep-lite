#ifndef DATABASE_H
#define DATABASE_H

#include "clip.h"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <optional>
#include <memory>

class Database {
public:
    Database();
    ~Database();
    
    bool open(const std::string& path);
    void close();
    
    // CRUD operations
    int64_t insert_clip(ClipType type, const std::string& content, 
                         const std::string& hash, uint32_t width, uint32_t height,
                         int64_t size, const std::string* source = nullptr);
    
    bool update_clip(int64_t id, ClipType type, const std::string& content,
                     const std::string& hash, int64_t size);
    
    bool delete_clip(int64_t id);
    
    std::vector<Clip> list_clips(ClipScope scope, const std::string& query = "");
    
    std::optional<std::pair<std::string, std::string>> get_clip(int64_t id);
    
    // Operations
    bool toggle_favorite(int64_t id);
    bool toggle_pinned(int64_t id);
    bool set_group(int64_t id, std::optional<int64_t> group_id);
    
    std::vector<Group> list_groups();
    Group create_group(const std::string& name);
    bool rename_group(int64_t id, const std::string& name);
    bool delete_group(int64_t id, bool delete_items);
    bool reorder_groups(const std::vector<int64_t>& ids);
    
    // Storage management
    bool enforce_storage_limit(int64_t max_bytes, std::vector<std::string>& removed_files);
    StorageUsage get_storage_usage();
    std::vector<std::string> clear_data(bool include_pinned, bool include_grouped, bool include_all);
    
    // Export/Import
    std::vector<int64_t> get_all_clip_ids();
    std::optional<RawClip> export_one(int64_t id);
    bool import_clip(const RawClip& clip);
    
private:
    sqlite3* db_;
    
    bool execute(const std::string& sql, sqlite3_stmt** stmt = nullptr);
    bool init_schema();
    void migrate();
};

enum class ClipScope {
    ALL,
    FAVORITES,
    GROUP
};

#endif // DATABASE_H
