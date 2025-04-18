#include "user_server.hpp"

UserServer::UserServer(int port, int concurrency) : BaseServer(port, concurrency) {}

void UserServer::_define_handler_funcs() {
    auto api_handler = std::make_shared<UserApiHandler>();
    auto db_manager = DatabaseManager::create_from_env();

    auto concurrency_manager = std::make_shared<ConcurrencyManager>();
    auto concurrency_protection_decorator =
        [concurrency_manager](const std::function<crow::response(const crow::request&)> func) {
            return concurrency_manager->concurrency_protection_decorator(func);
        };

    auto jwt_manager = std::make_shared<JwtManager>();
    auto jwt_protection_decorator =
        [jwt_manager](const std::function<crow::response(const crow::request&)> func,
                      const JwtAccessLevel& access_level) {
            return jwt_manager->jwt_protection_decorator(func, access_level);
        };

    auto COLLECTION_USERS = Constants::COLLECTION_USERS;

    _register_handler_func(
        "/signup",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) {
            return api_handler->insert_one_account_citizen(req, db_manager, COLLECTION_USERS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/create_admin_account",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) {
            return api_handler->insert_one_account_admin(req, db_manager, COLLECTION_USERS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::Admin,
        jwt_protection_decorator);
    _register_handler_func(
        "/login",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) {
            return api_handler->login(req, db_manager, COLLECTION_USERS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/get_profile_by_oid",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) {
            return api_handler->get_one_profile_by_oid(req, db_manager, COLLECTION_USERS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/update_profile_by_oid",
        [api_handler, db_manager, COLLECTION_USERS](const crow::request& req) {
            return api_handler->update_one_by_oid(req, db_manager, COLLECTION_USERS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::Personal,
        jwt_protection_decorator);
}