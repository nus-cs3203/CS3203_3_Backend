#ifndef BASE_API_HANDLER_H
#define BASE_API_HANDLER_H

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
};

#endif