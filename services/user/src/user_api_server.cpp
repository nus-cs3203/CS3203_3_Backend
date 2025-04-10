#include "user_api_server.hpp"
#include <iostream>

UserApiServer::UserApiServer(int port, int concurrency)
    : port(port), concurrency(concurrency)
{
    _define_handler_funcs();
}

void UserApiServer::_init_server() {
    app = std::make_unique<crow::App<CORS>>();
    for (const auto& handler : handler_funcs) {
        app->route_dynamic(handler.route).methods(handler.method)(handler.func);
    }
}

void UserApiServer::_register_handler_func(const std::string& route,
                                             const std::function<crow::response(const crow::request&)>& func,
                                             const crow::HTTPMethod& method) {
    HandlerFunc handler_func = { route, func, method };
    handler_funcs.push_back(handler_func);
}

void UserApiServer::_define_handler_funcs() {
    auto api_handler = std::make_shared<UserApiHandler>();
    auto db_manager = DatabaseManager::create_from_env();

    auto COLLECTION_USERS = Constants::COLLECTION_USERS;

    _register_handler_func(
        "/signup",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) { 
            return api_handler->insert_one(req, db_manager, COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );
    _register_handler_func(
        "/login",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) { 
            return api_handler->login(req, db_manager, COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );
    _register_handler_func(
        "/get_profile_by_oid",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) { 
            return api_handler->get_one_profile_by_oid(req, db_manager, COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );
    _register_handler_func(
        "/update_profile_by_oid",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) { 
            return api_handler->update_one_by_oid(req, db_manager, COLLECTION_USERS); 
        },
        crow::HTTPMethod::Post
    );
}

void UserApiServer::run() {
    bool should_restart = false;
    do {
        try {
            _init_server();

            app->loglevel(crow::LogLevel::Warning);
            app->concurrency(concurrency);

            std::cout << "Server starting on port " << port << std::endl;
            app->port(port).run();
        } catch (const std::exception& e) {
            std::cout << "Server error: " << e.what() << std::endl;
            std::cout << "Restaring the server." << std::endl;
            should_restart = true;
        } 
    } while (should_restart);

    std::cout << "Stopping the server.";
}
