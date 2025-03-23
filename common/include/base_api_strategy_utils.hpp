#ifndef BASE_API_STRATEGY_UTILS_H
#define BASE_API_STRATEGY_UTILS_H

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <unordered_set>

namespace BaseApiStrategyUtils {
    void validate_fields(const crow::request& req, std::initializer_list<std::string> required_fields);
    auto make_error_response(int status_code, const std::string& message) -> crow::response;
    auto make_success_response(int status_code, crow::json::wvalue data, const std::string& message) -> crow::response;

    auto parse_database_json_to_response_json(const crow::json::rvalue& rval_json) -> crow::json::wvalue;
    
    auto parse_request_json_to_database_bson(const crow::json::rvalue& rval_json) -> bsoncxx::document::value;
    auto parse_request_json_to_database_bson_single_primitive(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value;
    auto _parse_request_json_to_database_bson_single_primitive(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value;
    auto _parse_request_json_to_database_bson_single_primitive_clean_key(const std::string& key) -> std::string;
    auto _parse_request_json_to_database_bson_single_primitive_parse_inequality(const std::string& key, const bsoncxx::document::value& doc) -> bsoncxx::document::value;
    auto parse_request_json_to_database_bson_single_array(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value;
    auto parse_request_json_to_database_bson_single_object(const std::string& key, const crow::json::rvalue& rval_json) -> bsoncxx::document::value;

    auto parse_oid_str_to_oid_bson(const std::string& oid_str) -> bsoncxx::document::value;
    auto parse_date_str_to_date_bson(const std::string& date_str) -> bsoncxx::types::b_date;

    const std::string DEFAULT_KEY = "";
    const std::string GTE_SIGN = "_from_";
    const std::string LTE_SIGN = "_to_";

    const std::unordered_set<std::string> DATE_FIELDS = {
        "date", "created_utc", "date_created", "date_published", "date_closed", "date_submitted",
        GTE_SIGN + "date",  GTE_SIGN + "created_utc",  GTE_SIGN + "date_created",  GTE_SIGN + "date_published",  GTE_SIGN + "date_closed",  GTE_SIGN + "date_submitted", 
        LTE_SIGN + "date",  LTE_SIGN + "created_utc",  LTE_SIGN + "date_created",  LTE_SIGN + "date_published",  LTE_SIGN + "date_closed",  LTE_SIGN + "date_submitted"
    };
}

#endif

