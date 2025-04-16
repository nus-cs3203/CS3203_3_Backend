#ifndef UPDATER_API_HANDLER_H
#define UPDATER_API_HANDLER_H

#include <memory>
#include <string>

#include "crow.h"
#include "database_manager.hpp"
#include "reddit_manager.hpp"

class UpdaterApiHandler {
   public:
    UpdaterApiHandler();

    auto update_posts(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager)
        -> crow::response;

    auto run_analytics(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                       const std::string& collection_name) -> crow::response;

    auto retrieve_analytics(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager)
        -> crow::response;

    auto clear_analytics(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                         const std::string& collection_name) -> crow::response;

   private:
    std::shared_ptr<RedditManager> reddit_manager;
    EnvManager env_manager;
    std::string analytics_url;
};

#endif