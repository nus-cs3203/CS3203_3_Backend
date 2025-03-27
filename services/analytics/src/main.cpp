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
#include <mutex>

int main() {
    auto db_manager = DatabaseManager::create_from_env();
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    AnalyticsApiHandler analytics_api_handler;

    std::mutex api_mutex; 

    auto COLLECTION_CATEGORY_ANALYTICS = Constants::COLLECTION_CATEGORY_ANALYTICS;

    CROW_ROUTE(app, "/category_analytics/get_by_name").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_CATEGORY_ANALYTICS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_one_by_name(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
    });

    auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    CROW_ROUTE(app, "/complaints/get_statistics").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_COMPLAINTS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_complaints_statistics(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_statistics_over_time").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_COMPLAINTS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_complaints_statistics_over_time(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_statistics_grouped").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_COMPLAINTS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_complaints_statistics_grouped(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_statistics_grouped_over_time").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_COMPLAINTS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_complaints_statistics_grouped_over_time(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaints/get_statistics_grouped_by_sentiment_value").methods(crow::HTTPMethod::Post)
    ([db_manager, &analytics_api_handler, COLLECTION_COMPLAINTS, &api_mutex](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(api_mutex); 
        return analytics_api_handler.get_complaints_statistics_grouped_by_sentiment_value(req, db_manager, COLLECTION_COMPLAINTS);
    });

    // Run the server
    app.concurrency(25);
    app.port(8082).run();
    return 0;
}