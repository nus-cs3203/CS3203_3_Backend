#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "reddit.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"
#include <memory>
#include <string>
#include <vector>

class ApiHandler {
public:
    ApiHandler() = default;

    auto perform_realtime_update_complaints_analytics_from_reddit(const crow::request& req,
    std::shared_ptr<Database> db, std::shared_ptr<Reddit> reddit, const std::string& subreddit,
    const std::string& collection_name) -> crow::response;
private:

};

#endif