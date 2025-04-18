#include "base_api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <utility>
#include <vector>

#include "base_api_strategy_utils.hpp"
#include "crow.h"
#include "database_manager.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto BaseApiHandler::find_one(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<
        std::tuple<bsoncxx::document::value, mongocxx::options::find>(const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const bsoncxx::document::value&)> process_response_func)
    -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->find_one(collection_name, filter, option);

        if (!result.has_value()) {
            return BaseApiStrategyUtils::make_success_response(
                200, {},
                "Server processed get request successfully but no matching documents found");
        }

        auto response_data = process_response_func(result.value());

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed get request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::find(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::find,
                             bsoncxx::document::value>(const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(mongocxx::cursor&)> process_response_func) -> crow::response {
    try {
        auto filter_and_option_and_sort = process_request_func(req);
        auto filter = std::get<0>(filter_and_option_and_sort);
        auto option = std::get<1>(filter_and_option_and_sort);
        auto sort = std::get<2>(filter_and_option_and_sort);
        option.sort(sort.view());

        auto cursor = db_manager->find(collection_name, filter, option);

        auto response_data = process_response_func(cursor);

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed get request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::insert_one(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<
        std::tuple<bsoncxx::document::value, mongocxx::options::insert>(const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::insert_one&)> process_response_func)
    -> crow::response {
    try {
        auto document_and_option = process_request_func(req);
        auto document = std::get<0>(document_and_option);
        auto option = std::get<1>(document_and_option);

        auto result = db_manager->insert_one(collection_name, document, option);
        if (!result.has_value()) {
            throw std::runtime_error("Insertion failed!");
        }

        auto response_data = process_response_func(result.value());

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed insert request successfully.");
    } catch (const mongocxx::exception& e) {
        if (e.code().value() == 11000) {
            return BaseApiStrategyUtils::make_error_response(409, "Unique constraint violation!");
        } else {
            return BaseApiStrategyUtils::make_error_response(500, e.what());
        }
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::delete_one(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>(
        const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::delete_result&)> process_response_func)
    -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->delete_one(collection_name, filter, option);

        auto response_data = process_response_func(result.value());

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed delete request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::delete_many(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>(
        const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::delete_result&)> process_response_func)
    -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->delete_many(collection_name, filter, option);

        auto response_data = process_response_func(result.value());

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed delete request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::update_one(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<std::tuple<bsoncxx::document::value, bsoncxx::document::value,
                             mongocxx::options::update>(const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const mongocxx::result::update&)> process_response_func)
    -> crow::response {
    try {
        auto filter_and_update_doc_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_update_doc_and_option);
        auto update_doc = std::get<1>(filter_and_update_doc_and_option);
        auto option = std::get<2>(filter_and_update_doc_and_option);

        auto result = db_manager->update_one(collection_name, filter, update_doc, option);

        auto response_data = process_response_func(result.value());

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed update request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::count_documents(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<
        std::tuple<bsoncxx::document::value, mongocxx::options::count>(const crow::request&)>
        process_request_func,
    std::function<crow::json::wvalue(const long long int&)> process_response_func)
    -> crow::response {
    try {
        auto filter_and_option = process_request_func(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto count = db_manager->count_documents(collection_name, filter, option);

        auto response_data = process_response_func(count);

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed count_documents request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto BaseApiHandler::aggregate(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name,
    std::function<std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate>(
        const crow::request&)>
        process_request_func,
    std::function<mongocxx::pipeline(const std::vector<bsoncxx::document::value>&)>
        create_pipeline_func,
    std::function<crow::json::wvalue(const crow::request&, mongocxx::cursor&)>
        process_response_func) -> crow::response {
    try {
        auto documents_and_option = process_request_func(req);
        auto documents = std::get<0>(documents_and_option);
        auto option = std::get<1>(documents_and_option);

        auto pipeline = create_pipeline_func(documents);
        auto cursor = db_manager->aggregate(collection_name, pipeline, option);

        auto response_data = process_response_func(req, cursor);

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed aggregate request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}