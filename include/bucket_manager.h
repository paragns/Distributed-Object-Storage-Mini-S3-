#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class BucketManager {
public:
    explicit BucketManager(const std::string& storage_root);

    // Returns true if bucket was created, false if it already existed
    bool create_bucket(const std::string& name);

    // Returns true if bucket was deleted, false if it did not exist
    bool delete_bucket(const std::string& name);

    // Returns list of all bucket names
    std::vector<std::string> list_buckets() const;

    bool bucket_exists(const std::string& name) const;
    bool is_empty(const std::string& name) const;

private:
    fs::path root_;
};
