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
    for (const auto& sub_field : rval_json) {
        std::string sub_field_key = static_cast<std::string>(sub_field.key());
        if (sub_field.t() == crow::json::type::Object) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_object(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        } else if (sub_field.t() == crow::json::type::List) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_array(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        } else {
            auto sub_field_doc = parse_request_json_to_database_bson_single_primitive(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        }
    }
    
    return doc_builder.extract();
}

auto BaseApiStrategyUtils::parse_request_json_to_database_bson_single_primitive(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    switch (rval_json.t()) {
        case crow::json::type::String:
            if (DATE_FIELDS.find(key) != DATE_FIELDS.end()) {
                auto unix_ts_val = string_to_utc_unix_timestamp(static_cast<std::string>(rval_json.s()), Constants::DATETIME_FORMAT) * 1000;
                return make_document(kvp(key, bsoncxx::types::b_date{std::chrono::milliseconds(unix_ts_val)}));
            }
            return make_document(kvp(key, rval_json.s()));

        case crow::json::type::Number:
            if (rval_json.nt() == crow::json::num_type::Signed_integer || rval_json.nt() == crow::json::num_type::Unsigned_integer) {
                return make_document(kvp(key, rval_json.i()));
            }
            if (rval_json.nt() == crow::json::num_type::Floating_point || rval_json.nt() == crow::json::num_type::Double_precision_floating_point) {
                return make_document(kvp(key, rval_json.d()));
            }
            if (rval_json.nt() == crow::json::num_type::Null) {
                return make_document(kvp(key, bsoncxx::types::b_null{}));
            }
            throw std::runtime_error("Unknown crow::json::num_type!");

        case crow::json::type::True:
        case crow::json::type::False:
            return make_document(kvp(key, rval_json.b()));

        case crow::json::type::Null:
            return make_document(kvp(key, bsoncxx::types::b_null{}));

        default:
            throw std::runtime_error("Non-primitive crow::json::type!");
    }
}

auto BaseApiStrategyUtils::parse_request_json_to_database_bson_single_array(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    bsoncxx::builder::basic::array arr_builder;
    for (const auto& sub_field : const_cast<crow::json::rvalue&>(rval_json).lo()) {
        if (sub_field.t() == crow::json::type::Object) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_object(DEFAULT_KEY, sub_field);
            arr_builder.append(sub_field_doc[DEFAULT_KEY].get_value());
        } else if (sub_field.t() == crow::json::type::List) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_array(DEFAULT_KEY, sub_field);
            arr_builder.append(sub_field_doc[DEFAULT_KEY].get_value());
        } else {
            auto sub_field_doc = parse_request_json_to_database_bson_single_primitive(DEFAULT_KEY, sub_field);
            arr_builder.append(sub_field_doc[DEFAULT_KEY].get_value());
        }
    }
    bsoncxx::array::value arr_val = arr_builder.extract();
    return make_document(kvp(key, bsoncxx::types::b_array{arr_val.view()}));
}

auto BaseApiStrategyUtils::parse_request_json_to_database_bson_single_object(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    bsoncxx::builder::basic::document doc_builder;
    for (const auto& sub_field : rval_json) {
        std::string sub_field_key = static_cast<std::string>(sub_field.key());
        if (sub_field.t() == crow::json::type::Object) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_object(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        } else if (sub_field.t() == crow::json::type::List) {
            auto sub_field_doc = parse_request_json_to_database_bson_single_array(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        } else {
            auto sub_field_doc = parse_request_json_to_database_bson_single_primitive(sub_field_key, sub_field);
            auto sub_field_kvp = kvp(sub_field_key, sub_field_doc[sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        }
    }
    
    return make_document(kvp(key, doc_builder.extract()));
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

auto BaseApiStrategyUtils::parse_complaints_filter(const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    bsoncxx::builder::basic::document filter_builder{};

    if (rval_json.has("keyword")) {
        auto keyword = rval_json["keyword"].s();
        filter_builder.append(
            kvp("$text", 
                make_document(
                    kvp("$search", static_cast<std::string>(keyword))
                )
            )
        );
    }

    if (rval_json.has("category")) {
        auto category = rval_json["category"].s();
        filter_builder.append(kvp("category", static_cast<std::string>(category)));
    }

    if (rval_json.has("source")) {
        auto source = rval_json["source"].s();
        filter_builder.append(kvp("source", static_cast<std::string>(source)));
    }

    if (rval_json.has("start_date")) {
        auto start_date_str = static_cast<std::string>(rval_json["start_date"].s());
        auto start_date_bdate = parse_date_str_to_date_bson(start_date_str);
        filter_builder.append(
            kvp("date", 
                make_document(
                    kvp("$gte", start_date_bdate)
                )
            )
        );
    }

    if (rval_json.has("end_date")) {
        auto end_date_str = static_cast<std::string>(rval_json["end_date"].s());
        auto end_date_bdate = parse_date_str_to_date_bson(end_date_str);
        filter_builder.append(
            kvp("date", 
                make_document(
                    kvp("$lte", end_date_bdate)
                )
            )
        );
    }

    if (rval_json.has("min_sentiment")) {
        auto min_sentiment = rval_json["min_sentiment"].d();
        filter_builder.append(
            kvp("sentiment", 
                make_document(
                    kvp("$gte", min_sentiment)
                )
            )
        );
    }

    if (rval_json.has("max_sentiment")) {
        auto max_sentiment = rval_json["max_sentiment"].d();
        filter_builder.append(
            kvp("sentiment", 
                make_document(
                    kvp("$lte", max_sentiment)
                )
            )
        );
    }
    
    return filter_builder.extract();
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