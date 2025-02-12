#include "api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>

// POST /insert_one
auto ApiHandler::insert_one_post(const crow::request& req, Database& db) -> crow::response
{
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"post"})) {
            return make_error_response(400, "Invalid request format");
        }

        if (!validate_request(body["post"], {"title", "source", "category", "date", "sentiment"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto collection_name = "post";
        auto json_document = body["post"];

        auto bson_document = json_to_bson(json_document);

        auto result = db.insert_one(collection_name, bson_document.view());
        auto id = result->inserted_id().get_oid().value.to_string();

        std::cout << "Inserted to collection " << collection_name 
                  << ": " << json_document << " with id " << id << "." << std::endl;

        crow::json::wvalue response_data;
        response_data["_id"] = id;
        return make_success_response(200, response_data, "Document(s) inserted successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}