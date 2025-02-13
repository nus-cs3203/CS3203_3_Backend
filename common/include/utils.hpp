#ifndef UTILS_H
#define UTILS_H

#include <bsoncxx/builder/basic/document.hpp>
#include "crow.h"

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value;

bool validate_request(const crow::json::rvalue& body, std::initializer_list<std::string> required_fields);

crow::response make_error_response(int status_code, const std::string& message);

crow::response make_success_response(int status_code, crow::json::wvalue data, const std::string& message);

std::string utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp, const std::string& format);

#endif
