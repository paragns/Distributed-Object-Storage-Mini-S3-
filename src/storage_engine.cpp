#include "storage_engine.h"
#include <fstream>

StorageEngine::StorageEngine(const std::string& storage_root)
    : root_(storage_root) {}

fs::path StorageEngine::object_path(const std::string& bucket, const std::string& key) const {
    return root_ / bucket / key;
}

fs::path StorageEngine::chunk_path(const std::string& bucket, const std::string& key, size_t index) const {
    return root_ / bucket / (key + ".chunk_" + std::to_string(index));
}

bool StorageEngine::put_object(const std::string& bucket, const std::string& key, const std::string& data) {
    if (!fs::exists(root_ / bucket)) {
        return false;
    }

    if (data.size() <= CHUNK_SIZE) {
        // Small object — store as a single file
        std::ofstream file(object_path(bucket, key), std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return false;
        file.write(data.data(), data.size());
        return file.good();
    }

    // Large object — split into chunks
    size_t num_chunks = (data.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;
    for (size_t i = 0; i < num_chunks; i++) {
        std::ofstream file(chunk_path(bucket, key, i), std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return false;
        size_t start = i * CHUNK_SIZE;
        size_t len   = std::min(CHUNK_SIZE, data.size() - start);
        file.write(data.data() + start, len);
        if (!file.good()) return false;
    }
    return true;
}

bool StorageEngine::get_object(const std::string& bucket, const std::string& key, std::string& data) {
    // Check for chunked storage first
    if (fs::exists(chunk_path(bucket, key, 0))) {
        data.clear();
        for (size_t i = 0; ; i++) {
            fs::path path = chunk_path(bucket, key, i);
            if (!fs::exists(path)) break;
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open()) return false;
            data.append(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());
        }
        return true;
    }

    // Single file
    fs::path path = object_path(bucket, key);
    if (!fs::exists(path)) return false;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return true;
}

bool StorageEngine::delete_object(const std::string& bucket, const std::string& key) {
    // Check for chunked storage first
    if (fs::exists(chunk_path(bucket, key, 0))) {
        for (size_t i = 0; ; i++) {
            fs::path path = chunk_path(bucket, key, i);
            if (!fs::exists(path)) break;
            fs::remove(path);
        }
        return true;
    }

    // Single file
    fs::path path = object_path(bucket, key);
    if (!fs::exists(path)) return false;
    fs::remove(path);
    return true;
}

bool StorageEngine::object_exists(const std::string& bucket, const std::string& key) const {
    return fs::exists(object_path(bucket, key)) ||
           fs::exists(chunk_path(bucket, key, 0));
}

size_t StorageEngine::get_chunk_count(const std::string& bucket, const std::string& key) const {
    size_t count = 0;
    while (fs::exists(chunk_path(bucket, key, count))) count++;
    return count == 0 ? 1 : count;
}
