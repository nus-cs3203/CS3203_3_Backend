#include "api_handler.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "jwt_manager.hpp"
#include "database.hpp"

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
    auto db = std::make_shared<Database>(Database::create_from_env());
    auto jwt_manager = std::make_shared<JwtManager>(JwtManager());
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::Post)
    ([db, &api_handler](const crow::request& req) {
        return api_handler.signup(req, db);
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)
    ([db, &api_handler, &jwt_manager](const crow::request& req) {
        return api_handler.login(req, db, jwt_manager);
    });
    
    app.port(8085).run();
    return 0;
}