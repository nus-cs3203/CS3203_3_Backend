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
            return BaseApiStrategyUtils::make_success_response(200, {}, "Server processed get request successfully but no matching documents found");
        }

        auto response_data = process_response_func(result.value());
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed get request successfully.");
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
    std::function<crow::json::wvalue(mongocxx::cursor&)> process_response_func
) -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto cursor = db_manager->find(collection_name, filter, option);

        auto response_data = process_response_func(cursor);
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed get request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::insert_one(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name, 
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::insert>(const crow::request&)> process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::insert_one&)> process_response_func
) -> crow::response {
    try {
        auto document_and_option = process_request_func(req);
        auto document = std::get<0>(document_and_option);
        auto option = std::get<1>(document_and_option);

        auto result = db_manager->insert_one(collection_name, document, option);

        if (!result.has_value()) {
            throw std::runtime_error("Insertion failed!");
        }

        auto response_data = process_response_func(result.value());
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed insert request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::delete_one(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager, 
    const std::string& collection_name, 
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>(const crow::request&)> process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::delete_result&)> process_response_func
) -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->delete_one(collection_name, filter, option);

        auto response_data = process_response_func(result.value());
        
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed delete request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}