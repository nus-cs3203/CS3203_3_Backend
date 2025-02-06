#include "database.hpp"
#include "utils.hpp"
#include "api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    const std::string uri = argc > 1 ? argv[1] : "mongodb://127.0.0.1:27017";
    const std::string db_name = argc > 2 ? argv[2] : "CS3203";

    // Initialize Database
    Database db(uri, db_name);

    // Create Crow app
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    CROW_ROUTE(app, "/insert_one").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.insert_one(req, db);
    });

    CROW_ROUTE(app, "/insert_many").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.insert_many(req, db);
    });

    CROW_ROUTE(app, "/find_one").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.find_one(req, db);
    });

    CROW_ROUTE(app, "/find").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.find(req, db);
    });

    CROW_ROUTE(app, "/delete_one").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.delete_one(req, db);
    });

    CROW_ROUTE(app, "/delete_many").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.delete_many(req, db);
    });

    CROW_ROUTE(app, "/update_one").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.update_one(req, db);
    });

    CROW_ROUTE(app, "/update_many").methods(crow::HTTPMethod::Post)
    ([&db, &api_handler](const crow::request& req) {
        return api_handler.update_many(req, db);
    });

    // Run the server
    app.port(8081).multithreaded().run();
    return 0;
}
