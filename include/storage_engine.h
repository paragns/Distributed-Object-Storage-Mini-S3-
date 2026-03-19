#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class StorageEngine {
public:
    explicit StorageEngine(const std::string& storage_root);

    // Write bytes to bucket/key. Returns true on success.
    bool put_object(const std::string& bucket, const std::string& key, const std::string& data);

    // Read bytes from bucket/key into data. Returns true if object exists.
    bool get_object(const std::string& bucket, const std::string& key, std::string& data);

    // Delete object at bucket/key. Returns true if it existed.
    bool delete_object(const std::string& bucket, const std::string& key);

    bool object_exists(const std::string& bucket, const std::string& key) const;

private:
    fs::path root_;

    fs::path object_path(const std::string& bucket, const std::string& key) const;
};
