#include "base_api_strategy_utils.hpp"
#include "analytics_api_strategy.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>
#include <unordered_map>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto AnalyticsApiStrategy::process_request_func_get_one_by_name(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"name"});

    auto body = crow::json::load(req.body);
    auto filter = make_document(
        kvp("name", body["name"].s())
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto AnalyticsApiStrategy::process_request_func_get_complaints_statistics(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
    auto body = crow::json::load(req.body);
    auto filter = make_document();
    if (body.has("filter")) {
        filter = BaseApiStrategyUtils::parse_complaints_filter(body["filter"]);
    }

    std::vector<bsoncxx::document::value> documents = {filter};

    mongocxx::options::aggregate option;

    return std::make_tuple(documents, option);
}

auto AnalyticsApiStrategy::create_pipeline_func_get_complaints_statistics(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline {
    mongocxx::pipeline pipeline{};

    const auto &filter = documents[0];
    pipeline.match(filter.view());

    pipeline.group(
        make_document(
            kvp("_id", bsoncxx::types::b_null()),
            kvp("count", make_document(kvp("$sum", 1))),
            kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
        )
    );

    return pipeline;
}

auto AnalyticsApiStrategy::process_response_func_get_complaints_statistics(mongocxx::cursor& cursor) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    response_data["count"] = 0;
    response_data["avg_sentiment"] = 0;
    for (const auto& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);
        response_data["count"] = rval_json["count"];
        response_data["avg_sentiment"] = rval_json["avg_sentiment"];
    }
    return response_data;
}
