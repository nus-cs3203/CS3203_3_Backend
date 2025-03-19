#include "base_api_strategy.hpp"
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
    return find_one(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_one_by_oid, BaseApiStrategy::process_response_func_get_one);
}

auto ManagementApiHandler::get_all(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_all, ManagementApiStrategy::process_response_func_get);
}

auto ManagementApiHandler::get_by_daterange(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_by_daterange, ManagementApiStrategy::process_response_func_get);
}

auto ManagementApiHandler::get_many(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return find(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_get_many, ManagementApiStrategy::process_response_func_get);
}

auto ManagementApiHandler::delete_one_by_oid(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return delete_one(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_delete_one_by_oid, BaseApiStrategy::process_response_func_delete_one);
}

auto ManagementApiHandler::delete_many_by_oids(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return delete_many(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_delete_many_by_oids, BaseApiStrategy::process_response_func_delete_many);
}

auto ManagementApiHandler::update_one_by_oid(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name
) -> crow::response {
    return update_one(req, db_manager, collection_name, ManagementApiStrategy::process_request_func_update_one_by_oid, BaseApiStrategy::process_response_func_update_one);
}