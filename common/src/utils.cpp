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
