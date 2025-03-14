#include "base_api_strategy_utils.hpp"
#include "management_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto ManagementApiStrategy::process_request_func_get_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"oid"});

    auto body = crow::json::load(req.body);
    std::string oid_str = body["oid"].s();
    bsoncxx::oid oid{oid_str};
    auto filter = make_document(
        kvp("_id", oid)
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_response_func_get_one_by_oid(const bsoncxx::document::value& doc) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    auto doc_json = bsoncxx::to_json(doc);
    auto doc_rval = crow::json::load(doc_json);
    auto pased_doc_wval = BaseApiStrategyUtils::parse_database_json_to_response_json(doc_rval);
    response_data["document"] = std::move(pased_doc_wval);
    return response_data;
}

