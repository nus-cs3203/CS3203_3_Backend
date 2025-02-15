#include "api_handler.hpp"
#include "constants.hpp"
#include "database.hpp"
#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>

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
    const std::string MONGO_URI = read_env("MONGO_URI", Constants::MONGO_URI);
    const std::string DB_NAME   = read_env("DB_NAME", Constants::DB_NAME);

    Database db(MONGO_URI, DB_NAME);

    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    CROW_ROUTE(app, "/get_posts_grouped_by_sentiment_value").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped_by_sentiment_value(req, db);
    });

    CROW_ROUTE(app, "/get_posts_grouped_by_field").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped_by_field(req, db);
    });

    CROW_ROUTE(app, "/get_posts_grouped_by_field_over_time").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped_by_field_over_time(req, db);
    });

    CROW_ROUTE(app, "/get_posts_sorted_by_fields").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_sorted_by_fields(req, db);
    });

    // Run the server
    app.port(8082).run();
    return 0;
}
