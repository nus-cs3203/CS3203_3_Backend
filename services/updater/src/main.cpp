#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "reddit_manager.hpp"
#include "updater_api_handler.hpp"


#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <vector>

#include "api_handler.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "crow.h"
#include "reddit.hpp"

int main() {
    auto db_manager = DatabaseManager::create_from_env();

    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    UpdaterApiHandler updater_api_handler;

    CROW_ROUTE(app, "/update_posts").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler](const crow::request& req) {
        return updater_api_handler.update_posts(req, db_manager);
    });

    CROW_ROUTE(app, "/analytics/retrieve_all").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler](const crow::request& req) {
        return updater_api_handler.retrieve_analytics(req, db_manager);
    });

    const auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    CROW_ROUTE(app, "/complaint_analytics/run").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return updater_api_handler.run_analytics(req, db_manager, COLLECTION_COMPLAINTS);
    });

    CROW_ROUTE(app, "/complaint_analytics/clear").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_COMPLAINTS](const crow::request& req) {
        return updater_api_handler.clear_analytics(req, db_manager, COLLECTION_COMPLAINTS);
    });

    const auto COLLECTION_CATEGORY_ANALYTICS = Constants::COLLECTION_CATEGORY_ANALYTICS;

    CROW_ROUTE(app, "/category_analytics/run").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) {
        return updater_api_handler.run_analytics(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
    });

    CROW_ROUTE(app, "/category_analytics/clear").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_CATEGORY_ANALYTICS](const crow::request& req) {
        return updater_api_handler.clear_analytics(req, db_manager, COLLECTION_CATEGORY_ANALYTICS);
    });

    const auto COLLECTION_POLL_TEMPLATES = Constants::COLLECTION_POLL_TEMPLATES;

    CROW_ROUTE(app, "/poll_analytics/run").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_POLL_TEMPLATES](const crow::request& req) {
        return updater_api_handler.run_analytics(req, db_manager, COLLECTION_POLL_TEMPLATES);
    });

    CROW_ROUTE(app, "/poll_analytics/clear").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler, COLLECTION_POLL_TEMPLATES](const crow::request& req) {
        return updater_api_handler.clear_analytics(req, db_manager, COLLECTION_POLL_TEMPLATES);
    });

    app.port(8084).run();

    return 0;
}