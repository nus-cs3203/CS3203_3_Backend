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
    load_env_file(Constants::ENV_FILE_DEFAULT_PATH);

    const std::string MONGO_URI = read_env("MONGO_URI", Constants::MONGO_URI);
    const std::string DB_NAME   = read_env("DB_NAME", Constants::DB_NAME);

    auto db = std::make_shared<Database>(MONGO_URI, DB_NAME);

    Reddit reddit = Reddit::create_with_values_from_env();
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    const auto COLLECTION_CATEGORIES = Constants::COLLECTION_CATEGORIES;

    CROW_ROUTE(app, "/reddit/perform_update").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.reddit_perform_update(req, db, COLLECTION_CATEGORIES);
    });

    app.port(8084).run();
    return 0;
}