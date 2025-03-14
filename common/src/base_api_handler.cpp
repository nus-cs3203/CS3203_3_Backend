#include "base_api_handler.hpp"
#include "base_api_strategy_utils.hpp"
#include "database_manager.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <utility> 
#include <vector>

auto BaseApiHandler::find_one(
        const crow::request& req, 
        std::shared_ptr<DatabaseManager> db_manager, 
        const std::string& collection_name, 
        std::function<std::tuple<bsoncxx::document::value, mongocxx::options::find>(const crow::request&)> process_request_func,
        std::function<crow::json::wvalue(const bsoncxx::document::value&)> process_response_func
    ) -> crow::response {
    try {

        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->find_one(collection_name, filter, option);

        if (!result.has_value()) {
            return BaseApiStrategyUtils::make_success_response(200, {}, "No matching documents found");
        }

        auto response_data = process_response_func(result.value());
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Retrieved document successfully");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::find(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name, 
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::find>(const crow::request&)> process_request_func,
    std::function<crow::json::wvalue(const mongocxx::cursor&)> process_response_func
) -> crow::response {
    try {

        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto cursor = db_manager->find(collection_name, filter, option);

        auto response_data = process_response_func(cursor);
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Retrieved document successfully");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}