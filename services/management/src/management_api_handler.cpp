#include "management_api_handler.hpp"
#include "management_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

auto ManagementApiHandler::get_one_by_oid(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return ManagementApiHandler::find_one(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_one_by_oid, ManagementApiStrategy::process_response_func_get_one);
}

auto ManagementApiHandler::get_all(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return ManagementApiHandler::find(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_all, ManagementApiStrategy::process_response_func_get);
}