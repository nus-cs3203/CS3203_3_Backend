#include "api_handler.hpp"
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

int main(int argc, char* argv[]) {
    const std::string uri = argc > 1 ? argv[1] : "mongodb://127.0.0.1:27017";
    const std::string db_name = argc > 2 ? argv[2] : "CS3203";

    Database db(uri, db_name);

    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    CROW_ROUTE(app, "/get_posts_grouped_by_sentiment_value").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped_by_sentiment_value(req, db);
    });

    CROW_ROUTE(app, "/get_posts_grouped").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped(req, db);
    });

    CROW_ROUTE(app, "/get_posts_grouped_over_time").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_grouped_over_time(req, db);
    });

    CROW_ROUTE(app, "/get_posts_sorted").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.get_posts_sorted(req, db);
    });

    app.port(8082).multithreaded().run();
    return 0;
}