#include "api_handler.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "database.hpp"

#include "database_manager.hpp"
#include "management_api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>
#include <memory>

int main() {
    // auto db = std::make_shared<Database>(Database::create_from_env());
    auto db_manager = std::make_shared<DatabaseManager>(DatabaseManager::create_from_env());
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;
    ManagementApiHandler management_api_handler;

    const auto COLLECTION_CATEGORIES = Constants::COLLECTION_CATEGORIES;

    CROW_ROUTE(app, "/categories/get_count").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.count_documents(req, db_manager, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/get_all").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.get_all(req, db_manager, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/get_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.get_one_by_oid(req, db_manager, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/insert_one").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.insert_one(req, db_manager, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/delete_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.delete_one_by_oid(req, db_manager, COLLECTION_CATEGORIES);
    });

    CROW_ROUTE(app, "/categories/update_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_CATEGORIES](const crow::request& req) {
        return management_api_handler.update_one_by_oid(req, db_manager, COLLECTION_CATEGORIES);
    });

    const auto COLLECTION_POSTS = Constants::COLLECTION_POSTS;

    CROW_ROUTE(app, "/posts/get_count").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_POSTS](const crow::request& req) {
        return management_api_handler.count_documents(req, db_manager, COLLECTION_POSTS);
    });
    
    CROW_ROUTE(app, "/posts/get_by_daterange").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_POSTS](const crow::request& req) {
        return management_api_handler.get_by_daterange(req, db_manager, COLLECTION_POSTS);
    });

    const auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    CROW_ROUTE(app, "/complaints/get_count").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.count_documents(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.get_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_by_daterange").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.get_by_daterange(req, db_manager, COLLECTION_COMPLAINTS);
    });

    // CROW_ROUTE(app, "/complaints/search").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
    //     return api_handler.search(req, db, COLLECTION_COMPLAINTS, {"date"}, {false}, true);
    // });

    CROW_ROUTE(app, "/complaints/delete_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.delete_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/delete_many_by_oids").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.delete_many_by_oids(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/update_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &management_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return management_api_handler.update_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
    });

    app.port(8083).run();
    return 0;
}