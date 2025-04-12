#include "base_api_strategy_utils.hpp"
#include "constants.hpp"
#include "database_manager.hpp"
#include "date_utils.hpp"

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
            wval_json[key] = DateUtils::utc_unix_timestamp_to_string(value["$date"].i() / 1000, Constants::DATETIME_FORMAT);
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
            auto clean_sub_field_key = _parse_request_json_to_database_bson_single_primitive_clean_key(sub_field_key);
            auto sub_field_kvp = kvp(clean_sub_field_key, sub_field_doc[clean_sub_field_key].get_value());
            doc_builder.append(std::move(sub_field_kvp));
        }
    }
    
    return doc_builder.extract();
}

auto BaseApiStrategyUtils::parse_request_json_to_database_bson_single_primitive(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    auto doc = _parse_request_json_to_database_bson_single_primitive(key, rval_json);
    doc = _parse_request_json_to_database_bson_single_primitive_parse_inequality(key, doc);
    return doc;
}

auto BaseApiStrategyUtils::_parse_request_json_to_database_bson_single_primitive(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value {
    switch (rval_json.t()) {
        case crow::json::type::String:
            if (DATE_FIELDS.find(key) != DATE_FIELDS.end()) {
                auto unix_ts_val = DateUtils::string_to_utc_unix_timestamp(static_cast<std::string>(rval_json.s()), Constants::DATETIME_FORMAT) * 1000;
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

auto BaseApiStrategyUtils::_parse_request_json_to_database_bson_single_primitive_clean_key(const std::string& key) -> std::string {
    if (key.substr(0, LTE_SIGN.length()) == LTE_SIGN) {
        return key.substr(LTE_SIGN.length(), key.length());
    }

    if (key.substr(0, GTE_SIGN.length()) == GTE_SIGN) {
        return key.substr(GTE_SIGN.length(), key.length());
    }

    return key;

}

auto BaseApiStrategyUtils::_parse_request_json_to_database_bson_single_primitive_parse_inequality(const std::string& key, const bsoncxx::document::value& doc) -> bsoncxx::document::value {
    if (key.substr(0, LTE_SIGN.length()) == LTE_SIGN) {
        auto clean_key = key.substr(LTE_SIGN.length(), key.length());
        return make_document(kvp(clean_key, make_document(kvp("$lte", doc[key].get_value()))));
    }

    if (key.substr(0, GTE_SIGN.length()) == GTE_SIGN) {
        auto clean_key = key.substr(GTE_SIGN.length(), key.length());
        return make_document(kvp(clean_key, make_document(kvp("$gte", doc[key].get_value()))));
    }

    return doc;
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
    auto date_ts = DateUtils::string_to_utc_unix_timestamp(date_str, Constants::DATETIME_FORMAT) * 1000;
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