#include "user_api_server.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

UserApiServer::UserApiServer() {
    _register_handler_funcs();
}

void UserApiServer::init() {
    app = std::make_unique<crow::App<CORS>>();
    api_handler = std::make_unique<UserApiHandler>();
    db_manager = DatabaseManager::create_from_env();

    for (const auto& handler_func : handler_funcs) {
        app->route_dynamic(handler_func.route).methods(handler_func.method)(handler_func.func);
    }
}

void UserApiServer::run(const int& port, const int& concurrency) {
    while (true) {
        try {
            init();
            app->loglevel(crow::LogLevel::Warning);
            app->concurrency(concurrency);
            app->port(port).run();
        } catch (const std::exception& e) {
            std::cout << "Server error: " << e.what() << std::endl;
        }
    }
}

void UserApiServer::_register_handler_func(const std::string& route, const std::function<crow::response(const crow::request&)>& func, const crow::HTTPMethod& method) {
    HandlerFunc handler_func = { route, func, method };
    handler_funcs.push_back(handler_func);
}

void UserApiServer::_register_handler_funcs() {
    _register_handler_func(
        "/signup",
        [this](const crow::request& req) { 
            return this->api_handler->insert_one(req, this->db_manager, this->COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );

    _register_handler_func(
        "/login",
        [this](const crow::request& req) { 
            return this->api_handler->login(req, this->db_manager, this->COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );

    _register_handler_func(
        "/get_profile_by_oid",
        [this](const crow::request& req) { 
            return this->api_handler->get_one_profile_by_oid(req, this->db_manager, this->COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );

    _register_handler_func(
        "/update_profile_by_oid",
        [this](const crow::request& req) { 
            return this->api_handler->update_one_by_oid(req, this->db_manager, this->COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );
}
