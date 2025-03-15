#ifndef BASE_API_HANDLER_H
#define BASE_API_HANDLER_H

#include "base_api_strategy.hpp"
#include "database_manager.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

class BaseApiHandler {
public:
    auto find_one(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::find>(const crow::request&)> process_request_func,
        std::function<crow::json::wvalue(const bsoncxx::document::value&)> process_response_func
    ) -> crow::response;

    auto find(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::find>(const crow::request&)> process_request_func,
        std::function<crow::json::wvalue(mongocxx::cursor&)> process_response_func
    ) -> crow::response;

    auto insert_one(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::insert>(const crow::request&)> process_request_func = BaseApiStrategy::process_request_func_insert_one,
        std::function<crow::json::wvalue(const mongocxx::result::insert_one&)> process_response_func = BaseApiStrategy::process_response_func_insert_one
    ) -> crow::response;

    auto delete_one(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>(const crow::request&)> process_request_func,
        std::function<crow::json::wvalue(const mongocxx::result::delete_result&)> process_response_func
    ) -> crow::response;

    auto delete_many(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>(const crow::request&)> process_request_func,
        std::function<crow::json::wvalue(const mongocxx::result::delete_result&)> process_response_func
    ) -> crow::response;
};

#endif