#include "bucket_manager.h"

BucketManager::BucketManager(const std::string& storage_root)
    : root_(storage_root) {}

bool BucketManager::create_bucket(const std::string& name) {
    fs::path bucket_path = root_ / name;
    if (fs::exists(bucket_path)) {
        return false;
    }
    fs::create_directories(bucket_path);
    return true;
}

bool BucketManager::delete_bucket(const std::string& name) {
    fs::path bucket_path = root_ / name;
    if (!fs::exists(bucket_path)) {
        return false;
    }
    fs::remove_all(bucket_path);
    return true;
}

std::vector<std::string> BucketManager::list_buckets() const {
    std::vector<std::string> buckets;
    if (!fs::exists(root_)) {
        return buckets;
    }
    for (const auto& entry : fs::directory_iterator(root_)) {
        if (entry.is_directory()) {
            buckets.push_back(entry.path().filename().string());
        }
    }
    return buckets;
}

bool BucketManager::bucket_exists(const std::string& name) const {
    return fs::exists(root_ / name);
}
