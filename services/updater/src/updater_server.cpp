#include "updater_server.hpp"

UpdaterServer::UpdaterServer(int port, int concurrency) : BaseServer(port, concurrency) {}

void UpdaterServer::_define_handler_funcs() {
    auto api_handler = std::make_shared<UpdaterApiHandler>();
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

    _register_handler_func(
        "/update_posts",
        [api_handler, db_manager](const crow::request& req) {
            return api_handler->update_posts(req, db_manager);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/analytics/retrieve_all",
        [api_handler, db_manager](const crow::request& req) {
            return api_handler->update_posts(req, db_manager);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);

    const auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    _register_handler_func(
        "/complaint_analytics/run",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) {
            return api_handler->run_analytics(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);

    const auto COLLECTION_CATEGORY_ANALYTICS = Constants::COLLECTION_CATEGORY_ANALYTICS;

    _register_handler_func(
        "/category_analytics/run",
        [api_handler, db_manager, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) {
            return api_handler->run_analytics(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/category_analytics/clear",
        [api_handler, db_manager, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) {
            return api_handler->clear_analytics(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);

    const auto COLLECTION_POLL_TEMPLATES = Constants::COLLECTION_POLL_TEMPLATES;

    _register_handler_func(
        "/poll_analytics/run",
        [api_handler, db_manager, COLLECTION_POLL_TEMPLATES](const crow::request& req) {
            return api_handler->run_analytics(req, db_manager, COLLECTION_POLL_TEMPLATES);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);
    _register_handler_func(
        "/poll_analytics/clear",
        [api_handler, db_manager, COLLECTION_POLL_TEMPLATES](const crow::request& req) {
            return api_handler->clear_analytics(req, db_manager, COLLECTION_POLL_TEMPLATES);
        },
        crow::HTTPMethod::Post, concurrency_protection_decorator, JwtAccessLevel::None,
        jwt_protection_decorator);

}