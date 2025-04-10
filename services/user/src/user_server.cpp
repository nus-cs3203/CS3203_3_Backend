#include "user_server.hpp"
#include <iostream>

UserServer::UserServer(int port, int concurrency)
    : BaseServer(port, concurrency)
{
    define_handler_funcs();
}


void UserServer::define_handler_funcs() {
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