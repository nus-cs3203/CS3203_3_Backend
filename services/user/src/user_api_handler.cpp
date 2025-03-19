#include "management_api_strategy.hpp"
#include "user_api_handler.hpp"
#include "user_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

auto UserApiHandler::login(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find_one(req, db_manager, collection_name, UserApiStrategy::process_request_func_login, UserApiStrategy::process_response_func_login);
}

auto UserApiHandler::get_one_profile_by_oid(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find_one(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_one_by_oid, UserApiStrategy::process_response_func_get_one_profile_by_oid);
}