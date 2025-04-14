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
    auto db = std::make_shared<Database>(Database::create_from_env());

    std::shared_ptr<Reddit> reddit =
        std::make_shared<Reddit>(Reddit::create_with_values_from_env());

    crow::App<CORS> app;

    app.loglevel(crow::LogLevel::Warning);

    ApiHandler api_handler;

    auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    CROW_ROUTE(app, "/updater/realtime/complaints_analytics/reddit/singapore")
        .methods(crow::HTTPMethod::Post)(
            [db, reddit, COLLECTION_COMPLAINTS, &api_handler](const crow::request& req) {
                return api_handler.perform_realtime_update_complaints_analytics_from_reddit(
                    req, db, reddit, "singapore", COLLECTION_COMPLAINTS);
            });

    app.port(8084).run();
    return 0;
}