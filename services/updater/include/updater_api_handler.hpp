#ifndef UPDATER_API_HANDLER_H
#define UPDATER_API_HANDLER_H

#include "database_manager.hpp"
#include "reddit_manager.hpp"

#include "crow.h"

#include <memory>
#include <string>

class UpdaterApiHandler {
public:
    UpdaterApiHandler();

    auto update_posts(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager
    ) -> crow::response;

private:
    std::shared_ptr<RedditManager> reddit_manager;
};

#endif