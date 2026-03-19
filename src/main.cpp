#include <httplib.h>
#include <iostream>
#include "bucket_manager.h"

int main() {
    httplib::Server server;
    BucketManager bucket_mgr("storage/data");

    // Health check
    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\": \"ok\"}", "application/json");
    });

    // Create a bucket
    // PUT /bucket/{name}
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

    // Delete a bucket
    // DELETE /bucket/{name}
    server.Delete("/bucket/:name", [&](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.path_params.at("name");
        if (bucket_mgr.delete_bucket(name)) {
            res.status = 200;
            res.set_content("{\"message\": \"bucket deleted\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"error\": \"bucket not found\"}", "application/json");
        }
    });

    // List all buckets
    // GET /buckets
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

    // Catch-all for unrecognised routes
    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        res.status = 404;
        res.set_content("{\"error\": \"not found\"}", "application/json");
    });

    const int PORT = 8080;
    std::cout << "Mini S3 server starting on port " << PORT << std::endl;

    if (!server.listen("0.0.0.0", PORT)) {
        std::cerr << "Failed to start server on port " << PORT << std::endl;
        return 1;
    }

    return 0;
}
