#include "base_api_strategy_utils.hpp"
#include "constants.hpp"
#include "database_manager.hpp"
#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"

#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void BaseApiStrategyUtils::validate_fields(const crow::request& req, std::initializer_list<std::string> required_fields) {
    auto body = crow::json::load(req.body);
    if (!body and required_fields.size() > 0) {
        throw std::invalid_argument("Body is empty!");
    }
    for (const auto& field : required_fields) {
        if (!body.has(field)) {
            throw std::invalid_argument("Invalid request: missing " + field);
        }
    }
}

auto BaseApiStrategyUtils::parse_database_json_to_response_json(const crow::json::rvalue& rval_json) -> crow::json::wvalue {
    crow::json::wvalue wval_json;
    for (const auto& field: rval_json) {
        auto key = field.key();
        const auto& value = field; 
        
        // convert document date to string
        if (value.t() == crow::json::type::Object and value.has("$date")) {
            wval_json[key] = utc_unix_timestamp_to_string(value["$date"].i() / 1000, Constants::DATETIME_FORMAT);
            continue;
        }

        if (value.t() == crow::json::type::Object) {
            wval_json[key] = std::move(parse_database_json_to_response_json(value));
            continue;
        }

        wval_json[key] = value;
    }
    return wval_json;
}

auto BaseApiStrategyUtils::parse_request_json_to_database_bson(const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    bsoncxx::builder::basic::document doc_builder;
    for (const auto& field: rval_json) {
        auto doc_key = static_cast<std::string>(field.key());
        const auto& value = field; 
        
        if (doc_key == "date" or doc_key == "from_date" or doc_key == "start_date") {
            auto unix_ts_val = string_to_utc_unix_timestamp(static_cast<std::string>(value.s()), Constants::DATETIME_FORMAT) * 1000;
            bsoncxx::types::b_date b_date_val{std::chrono::milliseconds(unix_ts_val)};
            auto doc_val = b_date_val;
            doc_builder.append(kvp(doc_key, doc_val));
        } else if (value.t() == crow::json::type::Object) {
            auto doc_val = std::move(parse_request_json_to_database_bson(value));
            doc_builder.append(kvp(doc_key, doc_val));
        } else {
            std::ostringstream oss;
            oss << value;
            std::string json_str = oss.str();
            auto doc_val =  bsoncxx::from_json(json_str);
            doc_builder.append(kvp(doc_key, doc_val));
        }
    }
    return doc_builder.extract();
}

auto BaseApiStrategyUtils::make_error_response(int status_code, const std::string& message) -> crow::response {
    crow::json::wvalue res;
    res["success"] = false;
    res["message"] = message;
    return crow::response(status_code, res);
}

auto BaseApiStrategyUtils::make_success_response(int status_code, crow::json::wvalue data, const std::string& message) -> crow::response {
    data["success"] = true;
    data["message"] = message;
    return crow::response(status_code, data);
}