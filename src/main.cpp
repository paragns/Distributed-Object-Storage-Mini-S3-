#include <httplib.h>
#include <iostream>
#include "bucket_manager.h"
#include "storage_engine.h"

int main() {
    httplib::Server server;
    BucketManager bucket_mgr("storage/data");
    StorageEngine storage("storage/data");

    // Health check
    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\": \"ok\"}", "application/json");
    });

    // Create a bucket
    server.Put("/bucket/:name", [&](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.path_params.at("name");
        if (bucket_mgr.create_bucket(name)) {
            res.status = 201;
            res.set_content("{\"message\": \"bucket created\"}", "application/json");
        } else {
            res.status = 409;
            res.set_content("{\"error\": \"bucket already exists\"}", "application/json");
        }
    });

    // Delete a bucket (only if empty)
    server.Delete("/bucket/:name", [&](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.path_params.at("name");
        if (!bucket_mgr.bucket_exists(name)) {
            res.status = 404;
            res.set_content("{\"error\": \"bucket not found\"}", "application/json");
            return;
        }
        if (!bucket_mgr.is_empty(name)) {
            res.status = 409;
            res.set_content("{\"error\": \"bucket is not empty\"}", "application/json");
            return;
        }
        bucket_mgr.delete_bucket(name);
        res.status = 200;
        res.set_content("{\"message\": \"bucket deleted\"}", "application/json");
    });

    // List all buckets
    server.Get("/buckets", [&](const httplib::Request&, httplib::Response& res) {
        auto buckets = bucket_mgr.list_buckets();
        std::string json = "[";
        for (size_t i = 0; i < buckets.size(); ++i) {
            json += "\"" + buckets[i] + "\"";
            if (i + 1 < buckets.size()) json += ",";
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    // Upload an object
    // PUT /:bucket/:key
    server.Put("/:bucket/:key", [&](const httplib::Request& req, httplib::Response& res) {
        std::string bucket = req.path_params.at("bucket");
        std::string key    = req.path_params.at("key");

        if (!bucket_mgr.bucket_exists(bucket)) {
            res.status = 404;
            res.set_content("{\"error\": \"bucket not found\"}", "application/json");
            return;
        }

        if (storage.put_object(bucket, key, req.body)) {
            res.status = 200;
            res.set_content("{\"message\": \"object uploaded\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\": \"failed to write object\"}", "application/json");
        }
    });

    // Retrieve an object
    // GET /:bucket/:key
    server.Get("/:bucket/:key", [&](const httplib::Request& req, httplib::Response& res) {
        std::string bucket = req.path_params.at("bucket");
        std::string key    = req.path_params.at("key");

        if (!bucket_mgr.bucket_exists(bucket)) {
            res.status = 404;
            res.set_content("{\"error\": \"bucket not found\"}", "application/json");
            return;
        }

        std::string data;
        if (storage.get_object(bucket, key, data)) {
            // Detect content type from file extension
            std::string content_type = "application/octet-stream";
            auto ext_pos = key.rfind('.');
            if (ext_pos != std::string::npos) {
                std::string ext = key.substr(ext_pos);
                if      (ext == ".jpg" || ext == ".jpeg") content_type = "image/jpeg";
                else if (ext == ".png")                   content_type = "image/png";
                else if (ext == ".txt")                   content_type = "text/plain";
                else if (ext == ".json")                  content_type = "application/json";
            }
            res.status = 200;
            res.set_content(data, content_type);
        } else {
            res.status = 404;
            res.set_content("{\"error\": \"object not found\"}", "application/json");
        }
    });

    // Delete an object
    // DELETE /:bucket/:key
    server.Delete("/:bucket/:key", [&](const httplib::Request& req, httplib::Response& res) {
        std::string bucket = req.path_params.at("bucket");
        std::string key    = req.path_params.at("key");

        if (!bucket_mgr.bucket_exists(bucket)) {
            res.status = 404;
            res.set_content("{\"error\": \"bucket not found\"}", "application/json");
            return;
        }

        if (storage.delete_object(bucket, key)) {
            res.status = 200;
            res.set_content("{\"message\": \"object deleted\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\": \"object not found\"}", "application/json");
        }
    });

    // Catch-all: only fires for unmatched routes (body will be empty)
    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        if (res.body.empty()) {
            res.status = 404;
            res.set_content("{\"error\": \"not found\"}", "application/json");
        }
    });

    const int PORT = 8080;
    std::cout << "Mini S3 server starting on port " << PORT << std::endl;

    if (!server.listen("0.0.0.0", PORT)) {
        std::cerr << "Failed to start server on port " << PORT << std::endl;
        return 1;
    }

    return 0;
}
