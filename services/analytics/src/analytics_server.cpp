#include "analytics_server.hpp"

AnalyticsServer::AnalyticsServer(int port, int concurrency) : BaseServer(port, concurrency) {}

void AnalyticsServer::_define_handler_funcs() {
    auto api_handler = std::make_shared<AnalyticsApiHandler>();
    auto db_manager = DatabaseManager::create_from_env();
   
    auto concurrency_manager = std::make_shared<ConcurrencyManager>();
    auto concurrency_protection_decorator = [concurrency_manager](const std::function<crow::response(const crow::request&)> func) {
        return concurrency_manager->concurrency_protection_decorator(func);
    };

    auto jwt_manager = std::make_shared<JwtManager>();
    auto jwt_protection_decorator = [jwt_manager](const std::function<crow::response(const crow::request&)> func, const JwtAccessLevel& access_level) {
        return jwt_manager->jwt_protection_decorator(func, access_level);
    };

    auto COLLECTION_CATEGORY_ANALYTICS = Constants::COLLECTION_CATEGORY_ANALYTICS;

    _register_handler_func(
        "/category_analytics/get_by_name",
        [api_handler, db_manager, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) { 
            return api_handler->get_one_by_name(req, db_manager, COLLECTION_CATEGORY_ANALYTICS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );

    auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    _register_handler_func(
        "/complaints/get_statistics",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_complaints_statistics(req, db_manager, COLLECTION_COMPLAINTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_statistics_over_time",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_complaints_statistics_over_time(req, db_manager, COLLECTION_COMPLAINTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_statistics_grouped",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_complaints_statistics_grouped(req, db_manager, COLLECTION_COMPLAINTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_statistics_grouped_over_time",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_complaints_statistics_grouped_over_time(req, db_manager, COLLECTION_COMPLAINTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_statistics_grouped_by_sentiment_value",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_complaints_statistics_grouped_by_sentiment_value(req, db_manager, COLLECTION_COMPLAINTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator,
        JwtAccessLevel::None,
        jwt_protection_decorator
    );
}