#include "storage_engine.h"
#include <fstream>

StorageEngine::StorageEngine(const std::string& storage_root)
    : root_(storage_root) {}

fs::path StorageEngine::object_path(const std::string& bucket, const std::string& key) const {
    return root_ / bucket / key;
}

bool StorageEngine::put_object(const std::string& bucket, const std::string& key, const std::string& data) {
    fs::path path = object_path(bucket, key);

    // Bucket directory must exist
    if (!fs::exists(path.parent_path())) {
        return false;
    }

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file.write(data.data(), data.size());
    return file.good();
}

bool StorageEngine::get_object(const std::string& bucket, const std::string& key, std::string& data) {
    fs::path path = object_path(bucket, key);

    if (!fs::exists(path)) {
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return true;
}

bool StorageEngine::delete_object(const std::string& bucket, const std::string& key) {
    fs::path path = object_path(bucket, key);
    if (!fs::exists(path)) {
        return false;
    }
    fs::remove(path);
    return true;
}

bool StorageEngine::object_exists(const std::string& bucket, const std::string& key) const {
    return fs::exists(object_path(bucket, key));
}
