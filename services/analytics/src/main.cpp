#include "api_handler.hpp"
#include "cors.hpp"
#include "database.hpp"

#include "analytics_api_handler.hpp"
#include "cors.hpp"
#include "database_manager.hpp"

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

    // ApiHandler api_handler;
    AnalyticsApiHandler analytics_api_handler;

    auto COLLECTION_CATEGORY_ANALYTICS = Constants::COLLECTION_CATEGORY_ANALYTICS;

    CROW_ROUTE(app, "/get_category_analytics_by_name").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) {
        return analytics_api_handler.get_one_by_name(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
    });

    // CROW_ROUTE(app, "/get_complaints_statistics").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_statistics(req, db);
    // });

    // CROW_ROUTE(app, "/get_complaints_statistics_over_time").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_statistics_over_time(req, db);
    // });

    // CROW_ROUTE(app, "/get_complaints_grouped_by_sentiment_value").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_grouped_by_sentiment_value(req, db);
    // });

    // CROW_ROUTE(app, "/get_complaints_grouped_by_field").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_grouped_by_field(req, db);
    // });

    // CROW_ROUTE(app, "/get_complaints_grouped_by_field_over_time").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_grouped_by_field_over_time(req, db);
    // });

    // CROW_ROUTE(app, "/get_complaints_sorted_by_fields").methods(crow::HTTPMethod::Post)
    // ([db, &api_handler](const crow::request& req) {
    //     return api_handler.get_complaints_sorted_by_fields(req, db);
    // });

    // Run the server
    app.port(8082).run();
    return 0;
}