#include "base_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include <string>
#include <tuple>

#include "base_api_strategy_utils.hpp"
#include "crow.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto BaseApiStrategy::process_request_func_get_one(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"filter"});

    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);
    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto BaseApiStrategy::process_response_func_get_one(const bsoncxx::document::value& doc)
    -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto doc_json = bsoncxx::to_json(doc);
    auto doc_rval = crow::json::load(doc_json);
    auto pased_doc_wval = BaseApiStrategyUtils::parse_database_json_to_response_json(doc_rval);
    response_data["document"] = std::move(pased_doc_wval);
    return response_data;
}

auto BaseApiStrategy::process_request_func_insert_one(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    BaseApiStrategyUtils::validate_fields(req, {"document"});

    auto body = crow::json::load(req.body);
    auto document = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["document"]);
    mongocxx::options::insert option;

    return std::make_tuple(document, option);
}

auto BaseApiStrategy::process_response_func_insert_one(const mongocxx::result::insert_one& result)
    -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto oid = result.inserted_id().get_oid().value.to_string();
    response_data["oid"] = oid;
    return response_data;
}

auto BaseApiStrategy::process_response_func_delete_one(
    const mongocxx::result::delete_result& result) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto deleted_count = result.deleted_count();
    response_data["deleted_count"] = deleted_count;
    return response_data;
}

auto BaseApiStrategy::process_response_func_delete_many(
    const mongocxx::result::delete_result& result) -> crow::json::wvalue {
    return process_response_func_delete_one(result);
}

auto BaseApiStrategy::process_response_func_update_one(const mongocxx::result::update& result)
    -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto matched_count = result.matched_count();
    auto modified_count = result.modified_count();
    auto upserted_count = result.upserted_count();
    response_data["matched_count"] = matched_count;
    response_data["modified_count"] = modified_count;
    response_data["upserted_count"] = upserted_count;
    return response_data;
}

auto BaseApiStrategy::process_request_func_count_documents(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::count> {
    BaseApiStrategyUtils::validate_fields(req, {"filter"});

    auto body = crow::json::load(req.body);
    auto document = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);
    mongocxx::options::count option;

    return std::make_tuple(document, option);
}

auto BaseApiStrategy::process_response_func_count_documents(const long long int& count)
    -> crow::json::wvalue {
    crow::json::wvalue response_data;
    response_data["count"] = count;
    return response_data;
}