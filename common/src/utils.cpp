#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"

#include <cstdlib>
#include <iostream>

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value {
    std::ostringstream oss;
    oss << json_document;
    std::string json_str = oss.str();
    return bsoncxx::from_json(json_str);
}

auto validate_request(const crow::json::rvalue& body, std::initializer_list<std::string> required_fields) -> bool {
    if (!body) return false;
    for (const auto& field : required_fields) {
        if (!body.has(field)) return false;
    }
    return true;
}

auto make_error_response(int status_code, const std::string& message) -> crow::response {
    crow::json::wvalue res;
    res["success"] = false;
    res["message"] = message;
    return crow::response(status_code, res);
}

auto make_success_response(int status_code, crow::json::wvalue data, const std::string& message) -> crow::response {
    data["success"] = true;
    data["message"] = message;
    return crow::response(status_code, data);
}

auto utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp, const std::string& format) -> std::string {
    std::time_t time = static_cast<std::time_t>(utc_unix_timestamp);
    
    std::tm utc_time = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&utc_time, format.c_str());
    
    return oss.str();
}

auto string_to_utc_unix_timestamp(const std::string& datetime, const std::string& format) -> long long int {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, format.c_str());

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date/time string");
    }

    std::time_t time = timegm(&tm); 
    return static_cast<long long int>(time);
}


auto read_env(const std::string& key, const std::string& default_value) -> std::string {
    const char* env_var = std::getenv(key.c_str());
    if (!env_var) {
        std::cout << "Environment variable not found. Using default value." << std::endl;
        return default_value;
    }
    return static_cast<std::string>(env_var);
}