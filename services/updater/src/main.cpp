#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "reddit_manager.hpp"
#include "updater_api_handler.hpp"


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
    auto db_manager = DatabaseManager::create_from_env();

    // std::shared_ptr<Reddit> reddit = std::make_shared<Reddit>(Reddit::create_with_values_from_env());
    
    crow::App<CORS> app; 

    app.loglevel(crow::LogLevel::Warning);

    UpdaterApiHandler updater_api_handler;

    CROW_ROUTE(app, "/update_posts").methods(crow::HTTPMethod::Post)
    ([db_manager, &updater_api_handler](const crow::request& req) {
        return updater_api_handler.update_posts(req, db_manager);
    });

    app.port(8084).run();

    return 0;
}