#ifndef BASE_API_STRATEGY_UTILS_H
#define BASE_API_STRATEGY_UTILS_H

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>

namespace BaseApiStrategyUtils {
    void validate_fields(const crow::request& req, std::initializer_list<std::string> required_fields);
    auto parse_database_json_to_response_json(const crow::json::rvalue& rval_json) -> crow::json::wvalue;
    auto parse_request_json_to_database_bson(const crow::json::rvalue& rval_json) -> bsoncxx::document::value;
    auto make_error_response(int status_code, const std::string& message) -> crow::response;
    auto make_success_response(int status_code, crow::json::wvalue data, const std::string& message) -> crow::response;
}

#endif