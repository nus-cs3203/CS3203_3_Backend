#include "api_handler.hpp"
#include "constants.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto ApiHandler::test(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        // if (!validate_request(body, {"start_date", "end_date", "group_by_field"})) {
        //     return make_error_response(400, "Invalid request format");
        // }
        
        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Test.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}