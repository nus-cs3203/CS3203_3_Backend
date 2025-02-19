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

auto ApiHandler::get_complaint_by_oid(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"oid"})) {
            return make_error_response(400, "Invalid request format");
        }

        std::string oid_str = body["oid"].s();
        bsoncxx::oid oid{oid_str};

        auto filter = make_document(
            kvp("_id", oid)
        );

        auto result = db->find_one(Constants::COLLECTION_COMPLAINTS, filter);

        if (!result.has_value()) {
            return make_success_response(200, {}, "No matching documents found");
        }

        auto document_json = bsoncxx::to_json(result.value());
        auto document_wvalue = crow::json::load(document_json);

        crow::json::wvalue response_data;
        response_data[Constants::COLLECTION_COMPLAINTS] = document_wvalue;
        return make_success_response(200, response_data, "Complaint found successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}