#include <httplib.h>
#include <iostream>

int main() {
    httplib::Server server;

    // Health check endpoint
    // Clients (and load balancers) call this to verify the server is alive
    server.Get("/health", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("{\"status\": \"ok\"}", "application/json");
    });

    // Catch-all for unrecognised routes
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
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
