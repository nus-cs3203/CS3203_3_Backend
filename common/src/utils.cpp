#include "constants.hpp"
#include "database.hpp"
#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

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

auto json_date_to_bson_date(const crow::json::rvalue& json_date) -> bsoncxx::types::b_date {
    auto date_str = json_date.s();
    auto date_ts = string_to_utc_unix_timestamp(date_str, Constants::DATETIME_FORMAT) * 1000;
    bsoncxx::types::b_date date_bson{std::chrono::milliseconds(date_ts)};
    return date_bson;
}

void load_env_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not find or open .env file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Ignore comments
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            setenv(key.c_str(), value.c_str(), 1); // Set environment variable
        }
    }
    file.close();
}

auto get_utc_timestamp_now() -> long long int {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

auto get_utc_timestamp_one_day_ago() -> long long int {
    return get_utc_timestamp_now() - 86400;
}

auto create_empty_crow_json_rvalue() -> crow::json::rvalue {
    return crow::json::load("{}");
}