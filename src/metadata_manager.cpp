#include "metadata_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

MetadataManager::MetadataManager(const std::string& metadata_path)
    : path_(metadata_path) {}

// --- Serialization helpers ---

// Parse a single JSON object line:
// {"bucket":"photos","key":"img.jpg","size":1024,"created_at":1700000000}
static ObjectMetadata parse_record(const std::string& line) {
    ObjectMetadata obj;

    auto extract = [&](const std::string& field) -> std::string {
        std::string search = "\"" + field + "\":";
        auto pos = line.find(search);
        if (pos == std::string::npos) return "";
        pos += search.size();
        if (line[pos] == '"') {
            // string value
            pos++;
            auto end = line.find('"', pos);
            return line.substr(pos, end - pos);
        } else {
            // numeric value
            auto end = line.find_first_of(",}", pos);
            return line.substr(pos, end - pos);
        }
    };

    obj.bucket     = extract("bucket");
    obj.key        = extract("key");
    obj.size       = std::stoul(extract("size"));
    obj.created_at = std::stol(extract("created_at"));
    return obj;
}

static std::string serialize_record(const ObjectMetadata& obj) {
    return "{\"bucket\":\"" + obj.bucket +
           "\",\"key\":\"" + obj.key +
           "\",\"size\":"  + std::to_string(obj.size) +
           ",\"created_at\":" + std::to_string(obj.created_at) + "}";
}

// --- Core operations ---

std::vector<ObjectMetadata> MetadataManager::load() const {
    std::vector<ObjectMetadata> records;
    std::ifstream file(path_);
    if (!file.is_open()) return records;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            records.push_back(parse_record(line));
        }
    }
    return records;
}

void MetadataManager::save(const std::vector<ObjectMetadata>& records) const {
    std::ofstream file(path_, std::ios::trunc);
    for (const auto& r : records) {
        file << serialize_record(r) << "\n";
    }
}

void MetadataManager::put(const std::string& bucket, const std::string& key, size_t size) {
    auto records = load();

    // Update if already exists, otherwise append
    for (auto& r : records) {
        if (r.bucket == bucket && r.key == key) {
            r.size       = size;
            r.created_at = std::time(nullptr);
            save(records);
            return;
        }
    }

    ObjectMetadata obj;
    obj.bucket     = bucket;
    obj.key        = key;
    obj.size       = size;
    obj.created_at = std::time(nullptr);
    records.push_back(obj);
    save(records);
}

void MetadataManager::remove(const std::string& bucket, const std::string& key) {
    auto records = load();
    records.erase(
        std::remove_if(records.begin(), records.end(), [&](const ObjectMetadata& r) {
            return r.bucket == bucket && r.key == key;
        }),
        records.end()
    );
    save(records);
}

std::vector<ObjectMetadata> MetadataManager::list(const std::string& bucket) const {
    auto records = load();
    std::vector<ObjectMetadata> result;
    for (const auto& r : records) {
        if (r.bucket == bucket) {
            result.push_back(r);
        }
    }
    return result;
}

bool MetadataManager::exists(const std::string& bucket, const std::string& key) const {
    for (const auto& r : load()) {
        if (r.bucket == bucket && r.key == key) return true;
    }
    return false;
}
