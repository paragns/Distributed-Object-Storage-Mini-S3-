#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class StorageEngine {
public:
    // 64 bytes for demo — use 4 * 1024 * 1024 in production
    static constexpr size_t CHUNK_SIZE = 64;

    explicit StorageEngine(const std::string& storage_root);

    // Write bytes to bucket/key. Chunks automatically if data > CHUNK_SIZE.
    bool put_object(const std::string& bucket, const std::string& key, const std::string& data);

    // Read bytes from bucket/key, reassembling chunks if needed.
    bool get_object(const std::string& bucket, const std::string& key, std::string& data);

    // Delete object (single file or all chunks).
    bool delete_object(const std::string& bucket, const std::string& key);

    bool object_exists(const std::string& bucket, const std::string& key) const;

    // Returns number of chunks (1 if stored as single file)
    size_t get_chunk_count(const std::string& bucket, const std::string& key) const;

private:
    fs::path root_;

    fs::path object_path(const std::string& bucket, const std::string& key) const;
    fs::path chunk_path(const std::string& bucket, const std::string& key, size_t index) const;
};
