#ifndef BASE_API_STRATEGY_H
#define BASE_API_STRATEGY_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <tuple>
#include <vector>

namespace BaseApiStrategy {
    auto process_request_func_get_one(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find>;
    auto process_response_func_get_one(const bsoncxx::document::value& doc) -> crow::json::wvalue;
    
    auto process_request_func_insert_one(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert>;
    auto process_response_func_insert_one(const mongocxx::result::insert_one& result) -> crow::json::wvalue;

    auto process_response_func_delete_one(const mongocxx::result::delete_result& result) -> crow::json::wvalue;
    auto process_response_func_delete_many(const mongocxx::result::delete_result& result) -> crow::json::wvalue;

    auto process_response_func_update_one(const mongocxx::result::update& result) -> crow::json::wvalue;

    auto process_request_func_count_documents(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::count>;
    auto process_response_func_count_documents(const long long int& count) -> crow::json::wvalue; 
}

#endif