#include "base_api_strategy.hpp"
#include "base_api_strategy_utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto BaseApiStrategy::process_request_func_insert_one(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    BaseApiStrategyUtils::validate_fields(req, {"document"});

    auto body = crow::json::load(req.body);
    auto document = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["document"]);
    mongocxx::options::insert option;

    return std::make_tuple(document, option);
}

auto BaseApiStrategy::process_response_func_insert_one(const mongocxx::result::insert_one& result) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto oid = result.inserted_id().get_oid().value.to_string();
    response_data["oid"] = oid;
    return response_data;
}

auto BaseApiStrategy::process_response_func_delete_one(const mongocxx::result::delete_result& result) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto deleted_count = result.deleted_count();
    response_data["deleted_count"] = deleted_count;
    return response_data;
}