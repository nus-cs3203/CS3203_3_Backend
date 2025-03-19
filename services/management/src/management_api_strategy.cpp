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

    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_oid_str_to_oid_bson(body["oid"].s());

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_request_func_get_many(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value> {
    BaseApiStrategyUtils::validate_fields(req, {"filter", "page_size", "page_number"});

    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);

    auto page_size = body["page_size"].i();
    auto page_number = body["page_number"].i();

    if (page_size < 1) {
        throw std::invalid_argument("Invalid page_size < 1.");
    }

    if (page_number < 1) {
        throw std::invalid_argument("Invalid page_number < 1.");
    }

    mongocxx::options::find option;
    option.skip((page_number - 1) * page_size);
    option.limit(page_size);

    bsoncxx::document::value sort = make_document();
    if (body.has("sort")) {
        sort = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["sort"]);
    }

    return std::make_tuple(filter, option, sort);
}

auto ManagementApiStrategy::process_request_func_get_all(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value> {    
    auto filter = make_document();
    mongocxx::options::find option;
    bsoncxx::document::value sort = make_document();

    return std::make_tuple(filter, option, sort);
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

auto ManagementApiStrategy::process_request_func_get_by_daterange(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value> {
    BaseApiStrategyUtils::validate_fields(req, {"start_date", "end_date"});

    auto body = crow::json::load(req.body);
    auto start_date = BaseApiStrategyUtils::parse_date_str_to_date_bson(body["start_date"].s());
    auto end_date = BaseApiStrategyUtils::parse_date_str_to_date_bson(body["end_date"].s());

    bsoncxx::document::value filter = make_document(
        kvp("date", make_document(
            kvp("$gte", start_date),
            kvp("$lte", end_date)
        ))
    );

    mongocxx::options::find option;

    bsoncxx::document::value sort = make_document();

    return std::make_tuple(filter, option, sort);
}


auto ManagementApiStrategy::process_request_func_delete_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options> {
    BaseApiStrategyUtils::validate_fields(req, {"oid"});

    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_oid_str_to_oid_bson(body["oid"].s());

    mongocxx::options::delete_options option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_request_func_delete_many_by_oids(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options> {
    BaseApiStrategyUtils::validate_fields(req, {"oids"});
    
    auto body = crow::json::load(req.body);
    if (body["oids"].t() != crow::json::type::List) {
        throw std::invalid_argument("Field 'oids' must be an array!");
    }
    auto oid_jsons = body["oids"];
    bsoncxx::builder::basic::array oid_arr_builder;
    for (const auto& oid_json: oid_jsons.lo()) {
        std::string oid_str = oid_json.s();
        bsoncxx::oid oid{oid_str};
        oid_arr_builder.append(oid);
    }
    bsoncxx::array::value oid_arr = oid_arr_builder.extract();
    auto filter = make_document(
        kvp(
            "_id",
            make_document(
                kvp("$in", bsoncxx::types::b_array{oid_arr.view()})
            )
        )
    );

    mongocxx::options::delete_options option;

    return std::make_tuple(filter, option);
}

auto ManagementApiStrategy::process_request_func_update_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, bsoncxx::document::value, mongocxx::options::update> {
    BaseApiStrategyUtils::validate_fields(req, {"oid", "update_document"});

    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_oid_str_to_oid_bson(body["oid"].s());

    auto update_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["update_document"]);

    mongocxx::options::update option;

    return std::make_tuple(filter, update_doc, option);
}
