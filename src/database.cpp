#include "database.h"
#include "utilities.h"
#include <sstream>
#include <vector>

Database::Database() : db_(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        return false;
    }
    
    // Enable WAL mode
    execute("PRAGMA journal_mode = WAL");
    execute("PRAGMA synchronous = NORMAL");
    
    // Initialize schema
    init_schema();
    return true;
}

void Database::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Database::execute(const std::string& sql, sqlite3_stmt** stmt) {
    sqlite3_stmt* temp_stmt = nullptr;
    sqlite3_stmt* use_stmt = stmt ? stmt : &temp_stmt;
    
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, use_stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    int rc = sqlite3_step(*use_stmt);
    bool success = (rc == SQLITE_DONE || rc == SQLITE_ROW || rc == SQLITE_ERROR);
    
    if (!stmt) {
        sqlite3_finalize(temp_stmt);
    }
    
    return success;
}

bool Database::init_schema() {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS clips (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            kind TEXT NOT NULL,
            content TEXT NOT NULL,
            preview TEXT,
            hash TEXT NOT NULL UNIQUE,
            width INTEGER DEFAULT 0,
            height INTEGER DEFAULT 0,
            size INTEGER NOT NULL,
            favorite INTEGER DEFAULT 0,
            fav_at INTEGER,
            pinned INTEGER DEFAULT 0,
            pinned_at INTEGER,
            source TEXT,
            group_id INTEGER,
            created_at INTEGER NOT NULL,
            updated_at INTEGER NOT NULL,
            use_count INTEGER DEFAULT 1
        );
        
        CREATE TABLE IF NOT EXISTS clipboard_groups (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            sort_order INTEGER NOT NULL DEFAULT 0,
            created_at INTEGER NOT NULL
        );
        
        CREATE INDEX IF NOT EXISTS idx_clips_order ON clips(
            pinned DESC, pinned_at DESC, updated_at DESC
        );
        
        CREATE INDEX IF NOT EXISTS idx_clips_hash ON clips(hash);
        CREATE INDEX IF NOT EXISTS idx_clips_group ON clips(group_id);
        CREATE INDEX IF NOT EXISTS idx_clips_loose ON clips(updated_at) 
            WHERE NOT (favorite = 1 OR pinned = 1 OR group_id IS NOT NULL);
    )";
    
    char* err_msg = nullptr;
    int result = sqlite3_exec(db_, schema, nullptr, nullptr, &err_msg);
    
    if (result != SQLITE_OK) {
        if (err_msg) {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
        return false;
    }
    
    return true;
}

int64_t Database::insert_text(const std::string& content, const std::string& hash, int64_t size) {
    std::ostringstream sql_stream;
    sql_stream << "INSERT INTO clips (kind, content, preview, hash, size, created_at, updated_at) "
               << "VALUES ('" << KIND_TEXT << "', '" << content.replace(content.find('\''), std::string::npos, "''") 
               << "', '" << content.substr(0, PREVIEW_CHARS).replace(content.substr(0, PREVIEW_CHARS).find('\''), std::string::npos, "''")
               << "', '" << hash.replace(hash.find('\''), std::string::npos, "''") 
               << "', " << size 
               << ", " << now_ms() << ", " << now_ms() << ")"
               << " ON CONFLICT(hash) DO UPDATE SET updated_at = excluded.updated_at";
    
    execute(sql_stream.str());
    
    return last_insert_rowid();
}

int64_t Database::last_insert_rowid() {
    sqlite3_int64 rowid = sqlite3_last_insert_rowid(db_);
    return static_cast<int64_t>(rowid);
}

std::optional<std::pair<std::string, std::string>> Database::get_text(int64_t id) {
    const char* sql = "SELECT kind, content FROM clips WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_int64(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* kind = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return std::make_pair(kind, std::string(content));
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<int64_t> Database::find_by_hash(const std::string& hash) {
    const char* sql = "SELECT id FROM clips WHERE hash = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, hash.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }
    
    sqlite3_finalize(stmt);
    return std::nullopt;
}

bool Database::touch(int64_t id) {
    const char* sql = "UPDATE clips SET updated_at = ?, use_count = use_count + 1 WHERE id = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int64(stmt, 1, now_ms());
    sqlite3_bind_int64(stmt, 2, id);
    
    bool result = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    
    return result;
}

StorageUsage Database::get_storage_usage() {
    StorageUsage usage{0, 0};
    
    const char* sql = 
        "SELECT "
        "COALESCE(SUM(CASE WHEN favorite = 0 AND pinned = 0 AND group_id IS NULL THEN size ELSE 0 END), 0),"
        "COALESCE(SUM(CASE WHEN favorite = 1 OR pinned = 1 OR group_id IS NOT NULL THEN size ELSE 0 END), 0) "
        "FROM clips";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return usage;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        usage.ordinary_bytes = sqlite3_column_int64(stmt, 0);
        usage.protected_bytes = sqlite3_column_int64(stmt, 1);
    }
    
    sqlite3_finalize(stmt);
    return usage;
}

bool Database::enforce_storage_limit(int64_t max_bytes) {
    if (max_bytes <= 0) {
        return true;
    }
    
    StorageUsage usage = get_storage_usage();
    if (usage.ordinary_bytes <= max_bytes) {
        return true;
    }
    
    // Delete oldest loose items until under limit
    const char* delete_sql = 
        "DELETE FROM clips WHERE id IN ("
        "  SELECT id FROM clips "
        "  WHERE favorite = 0 AND pinned = 0 AND group_id IS NULL "
        "  ORDER BY updated_at ASC, id ASC "
        "  LIMIT 10"  // Delete in batches of 10
        ")";
    
    for (int i = 0; i < 10; i++) {
        execute(delete_sql);
        usage = get_storage_usage();
        if (usage.ordinary_bytes <= max_bytes) {
            break;
        }
    }
    
    return true;
}

std::vector<Group> Database::list_groups() {
    std::vector<Group> groups;
    
    const char* sql = "SELECT id, name, sort_order FROM clipboard_groups ORDER BY sort_order, id";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return groups;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int64_t sort_order = sqlite3_column_int64(stmt, 2);
        
        groups.emplace_back(id, std::string(name), sort_order);
    }
    
    sqlite3_finalize(stmt);
    return groups;
}

Group Database::create_group(const std::string& name) {
    int64_t next_order = 0;
    const char* order_sql = "SELECT COALESCE(MAX(sort_order), -1) + 1 FROM clipboard_groups";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, order_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            next_order = sqlite3_column_int64(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    std::ostringstream sql_stream;
    sql_stream << "INSERT INTO clipboard_groups (name, sort_order, created_at) "
               << "VALUES ('" << name.replace(name.find('\''), std::string::npos, "''") 
               << "', " << next_order << ", " << now_ms() << ")";
    
    execute(sql_stream.str());
    
    return Group(last_insert_rowid(), name, next_order);
}
