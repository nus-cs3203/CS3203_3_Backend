#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"

#include <iostream>

// Helper function to convert document from json to bson
auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value {
    std::ostringstream oss;
    oss << json_document;
    std::string json_str = oss.str();
    return bsoncxx::from_json(json_str);
}

// Helper function to validate required fields in a JSON request
bool validate_request(const crow::json::rvalue& body, std::initializer_list<std::string> required_fields) {
    if (!body) return false;
    for (const auto& field : required_fields) {
        if (!body.has(field)) return false;
    }
    return true;
}

// Helper function to create error responses
crow::response make_error_response(int status_code, const std::string& message) {
    crow::json::wvalue res;
    res["success"] = false;
    res["message"] = message;
    return crow::response(status_code, res);
}

// Helper function to create success responses
crow::response make_success_response(int status_code, crow::json::wvalue data, const std::string& message) {
    data["success"] = true;
    data["message"] = message;
    return crow::response(status_code, data);
}

std::string utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp, const std::string& format) {
    std::time_t time = static_cast<std::time_t>(utc_unix_timestamp);
    
    std::tm utc_time = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&utc_time, format.c_str());
    
    return oss.str();
}

long long int string_to_utc_unix_timestamp(const std::string& datetime, const std::string& format) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, format.c_str()); // Parse time

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date/time string");
    }

    // Convert to Unix timestamp (seconds)
    std::time_t time = timegm(&tm); // Use timegm for UTC, mktime for local time
    return static_cast<long long int>(time);
}