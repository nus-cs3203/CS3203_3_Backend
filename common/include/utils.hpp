#ifndef UTILS_H
#define UTILS_H

#include "constants.hpp"
#include "database.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include "crow.h"

#include <string>
#include <vector>

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value;

auto json_date_to_bson_date(const crow::json::rvalue& json_document) -> bsoncxx::types::b_date;

auto validate_request(const crow::json::rvalue& body, std::initializer_list<std::string> required_fields) -> bool;

auto make_error_response(int status_code, const std::string& message) -> crow::response;

auto make_success_response(int status_code, crow::json::wvalue data, const std::string& message) -> crow::response;

auto utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp, const std::string& format) -> std::string;

auto string_to_utc_unix_timestamp(const std::string& datetime, const std::string& format) -> long long int;

auto read_env(const std::string& key, const std::string& default_value = "") -> std::string;

void load_env_file(const std::string& filename = Constants::ENV_FILE_DEFAULT_PATH);

#endif
