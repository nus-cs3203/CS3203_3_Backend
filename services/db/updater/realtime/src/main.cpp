#include "api_handler.hpp"
#include "constants.hpp"
#include "reddit.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>
#include <memory>

struct CORS {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        if (req.method == "OPTIONS"_method) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.code = 204;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

int main() {
    load_env_file();

    const std::string MONGO_URI = read_env("MONGO_URI", Constants::MONGO_URI);
    const std::string DB_NAME   = read_env("DB_NAME", Constants::DB_NAME);

    auto db = std::make_shared<Database>(MONGO_URI, DB_NAME);

    std::shared_ptr<Reddit> reddit = std::make_shared<Reddit>(Reddit::create_with_values_from_env());
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    auto COLLECTION_COMPLAINTS = Constants::COLLECTION_CATEGORIES;

    CROW_ROUTE(app, "/updater/realtime/complaints_analytics/reddit/singapore").methods(crow::HTTPMethod::Post)
    ([db, reddit, COLLECTION_COMPLAINTS, &api_handler](const crow::request& req) {
        return api_handler.perform_realtime_update_complaints_analytics_from_reddit(req, db, reddit, "singapore", COLLECTION_COMPLAINTS);
    });

    app.port(8084).run();
    return 0;
}