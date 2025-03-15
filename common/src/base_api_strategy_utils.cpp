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
        auto key = static_cast<std::string>(field.key());
        auto value = field;

        if (value.t() == crow::json::type::Object) {
            auto doc_value = std::move(parse_request_json_to_database_bson(value));
            doc_builder.append(kvp(key, doc_value));
        } else if (value.t() == crow::json::type::String) {
            if (DATE_FIELDS.find(key) != DATE_FIELDS.end()) {
                auto unix_ts_val = string_to_utc_unix_timestamp(static_cast<std::string>(value.s()), Constants::DATETIME_FORMAT) * 1000;
                bsoncxx::types::b_date doc_val{std::chrono::milliseconds(unix_ts_val)};
                doc_builder.append(kvp(key, doc_val));
            } else {
                auto doc_val = value.s();
                doc_builder.append(kvp(key, doc_val));
            }
        } else if (value.t() == crow::json::type::Number) {
            if (value.nt() == crow::json::num_type::Signed_integer or value.nt() == crow::json::num_type::Unsigned_integer) {
                auto doc_value = value.i();
                doc_builder.append(kvp(key, doc_value));
            } else if (value.nt() == crow::json::num_type::Floating_point or value.nt() == crow::json::num_type::Double_precision_floating_point) {
                auto doc_value = value.d();
                doc_builder.append(kvp(key, doc_value));
            } else {
                throw std::runtime_error("Number is not signed integer, unsigned integer, floating or double! It may be null!");
            }
        } else if (value.t() == crow::json::type::True or value.t() == crow::json::type::False) {
            auto doc_value = value.b();
            doc_builder.append(kvp(key, doc_value));
        } else if (value.t() == crow::json::type::List) {
            // TODO: parse nested list
            bsoncxx::builder::basic::array arr_builder;
            for (auto sub_rval_json: value.lo()) {
                auto doc_value = sub_rval_json.s();
                arr_builder.append(doc_value);
            }
            bsoncxx::array::value arr_val = arr_builder.extract();
            doc_builder.append(kvp(key, bsoncxx::types::b_array{arr_val.view()}));
        } else if (value.t() == crow::json::type::Null) {
            bsoncxx::types::b_null doc_value{};
            doc_builder.append(kvp(key, doc_value));
        } else {
            throw std::runtime_error("Unrecognized crow::json::rvalue type!");
        }
    }
    return doc_builder.extract();
}

auto BaseApiStrategyUtils::parse_oid_str_to_oid_bson(const std::string& oid_str) -> bsoncxx::document::value {
    bsoncxx::oid oid{oid_str};
    return make_document(
        kvp("_id", oid)
    );
}

auto BaseApiStrategyUtils::parse_date_str_to_date_bson(const std::string& date_str) -> bsoncxx::types::b_date {
    auto date_ts = string_to_utc_unix_timestamp(date_str, Constants::DATETIME_FORMAT) * 1000;
    bsoncxx::types::b_date date_bson{std::chrono::milliseconds(date_ts)};
    return date_bson;
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