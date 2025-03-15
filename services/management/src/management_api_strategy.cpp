#include "base_api_strategy_utils.hpp"
#include "management_api_strategy.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto ManagementApiStrategy::process_request_func_get_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"oid"});

    auto filter = _parse_oid(req, "oid");

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_response_func_get_one(const bsoncxx::document::value& doc) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto doc_json = bsoncxx::to_json(doc);
    auto doc_rval = crow::json::load(doc_json);
    auto pased_doc_wval = BaseApiStrategyUtils::parse_database_json_to_response_json(doc_rval);
    response_data["document"] = std::move(pased_doc_wval);
    return response_data;
}

auto ManagementApiStrategy::process_request_func_get_all(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {    
    auto filter = make_document();
    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_response_func_get(mongocxx::cursor& cursor) -> crow::json::wvalue {
    crow::json::wvalue response_data;

    std::vector<crow::json::wvalue> documents;
    for (auto&& doc: cursor) {
        auto doc_json = bsoncxx::to_json(doc);
        auto doc_rval = crow::json::load(doc_json);
        auto pased_doc_wval = BaseApiStrategyUtils::parse_database_json_to_response_json(doc_rval);
        documents.push_back(std::move(pased_doc_wval));
    }

    response_data["documents"] = std::move(documents);
    return response_data;
}

auto ManagementApiStrategy::process_request_func_get_by_daterange(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"start_date", "end_date"});

    auto body = crow::json::load(req.body);
    auto start_date = json_date_to_bson_date(body["start_date"]);
    auto end_date = json_date_to_bson_date(body["end_date"]);

    bsoncxx::document::value filter = make_document(
        kvp("date", make_document(
            kvp("$gte", start_date),
            kvp("$lte", end_date)
        ))
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}


auto ManagementApiStrategy::process_request_func_delete_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options> {
    BaseApiStrategyUtils::validate_fields(req, {"oid"});

    auto filter = _parse_oid(req, "oid");

    mongocxx::options::delete_options option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::_parse_oid(const crow::request& req, const std::string& key) -> bsoncxx::document::value {
    auto body = crow::json::load(req.body);
    std::string oid_str = body[key].s();
    bsoncxx::oid oid{oid_str};
    return make_document(
        kvp("_id", oid)
    );
}
