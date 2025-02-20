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
    const std::string MONGO_URI = read_env("MONGO_URI", Constants::MONGO_URI);
    const std::string DB_NAME   = read_env("DB_NAME", Constants::DB_NAME);

    auto db = std::make_shared<Database>(MONGO_URI, DB_NAME);
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    const auto COLLECTION_CATEGORIES = Constants::COLLECTION_CATEGORIES;

    CROW_ROUTE(app, "/categories/get_all").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.get_all(req, db, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/get_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.get_by_oid(req, db, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/insert_one").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.insert_one(req, db, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/delete_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.delete_by_oid(req, db, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/update_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return api_handler.update_by_oid(req, db, COLLECTION_CATEGORIES);
    });

    const auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    CROW_ROUTE(app, "/complaints/get_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return api_handler.get_by_oid(req, db, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/delete_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return api_handler.delete_by_oid(req, db, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/delete_many_by_oids").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return api_handler.delete_many_by_oids(req, db, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/update_by_oid").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return api_handler.update_by_oid(req, db, COLLECTION_COMPLAINTS);
    });

    app.port(8083).run();
    return 0;
}