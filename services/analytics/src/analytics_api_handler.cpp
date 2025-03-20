#include "base_api_strategy.hpp"
#include "analytics_api_handler.hpp"
#include "analytics_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

auto AnalyticsApiHandler::get_one_by_name(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find_one(req, db_manager, collection_name, AnalyticsApiStrategy::process_request_func_get_one_by_name, BaseApiStrategy::process_response_func_get_one);
}
