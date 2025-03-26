#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "jwt_manager.hpp"
#include "user_api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

int main() {
    auto db_manager = DatabaseManager::create_from_env();
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    UserApiHandler user_api_handler;

    std::mutex api_mutex; 

    const auto COLLECTION_USERS = Constants::COLLECTION_USERS;

    CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::Post)
    ([db_manager, &user_api_handler, COLLECTION_USERS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return user_api_handler.insert_one(req, db_manager, COLLECTION_USERS);
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)
    ([db_manager, &user_api_handler, COLLECTION_USERS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return user_api_handler.login(req, db_manager, COLLECTION_USERS);
    });

    CROW_ROUTE(app, "/get_profile_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &user_api_handler, COLLECTION_USERS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return user_api_handler.get_one_profile_by_oid(req, db_manager, COLLECTION_USERS);
    });

    CROW_ROUTE(app, "/update_profile_by_oid").methods(crow::HTTPMethod::Post)
    ([db_manager, &user_api_handler, COLLECTION_USERS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return user_api_handler.update_one_by_oid(req, db_manager, COLLECTION_USERS);
    });

    app.concurrency(10);
    app.port(8085).run();
    return 0;
}