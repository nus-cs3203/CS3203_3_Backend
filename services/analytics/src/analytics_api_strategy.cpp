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