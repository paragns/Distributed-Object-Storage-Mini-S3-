#pragma once

#include <string>
#include <vector>
#include <ctime>

struct ObjectMetadata {
    std::string bucket;
    std::string key;
    size_t      size;
    std::time_t created_at;
};

class MetadataManager {
public:
    explicit MetadataManager(const std::string& metadata_path);

    // Record metadata when an object is uploaded
    void put(const std::string& bucket, const std::string& key, size_t size);

    // Remove metadata when an object is deleted
    void remove(const std::string& bucket, const std::string& key);

    // List all objects in a bucket
    std::vector<ObjectMetadata> list(const std::string& bucket) const;

    // Check if metadata exists for a given object
    bool exists(const std::string& bucket, const std::string& key) const;

private:
    std::string path_;

    std::vector<ObjectMetadata> load() const;
    void save(const std::vector<ObjectMetadata>& records) const;
};
