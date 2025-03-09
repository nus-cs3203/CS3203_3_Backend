#include "api_handler.hpp"
#include "constants.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto ApiHandler::signup(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"name", "email", "password"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto name = body["name"].s();
        auto email = body["email"].s();
        auto password = body["password"].s();

        auto document = make_document(
            kvp("name", name),
            kvp("email", email),
            kvp("password", password),
            kvp("role", Constants::USERS_ROLE_CITIZEN)
        );

        try {
            db->insert_one(Constants::COLLECTION_USERS, document);
        } catch (const mongocxx::exception& e) {
            if (e.code().value() == 11000) { 
                return make_error_response(409, "Email already in use.");
            } else {
                return make_error_response(500, std::string("DB Insertion error: ") + e.what());
            }
        }
        
        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Account created successfully.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::login(const crow::request& req, std::shared_ptr<Database> db, std::shared_ptr<JwtManager> jwt_manager) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"email", "password"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto email = body["email"].s();
        auto password = body["password"].s();

        auto filter = make_document(
            kvp("email", email),
            kvp("password", password)
        );

        auto result = db->find_one(Constants::COLLECTION_USERS, filter.view());

        if (!result.has_value()) {
            return make_error_response(401, "Account does not exist");
        }

        auto document_json = bsoncxx::to_json(result.value());
        auto document_rvalue = crow::json::load(document_json);
        
        auto oid = document_rvalue["_id"]["$oid"].s();

        auto role = static_cast<std::string>(document_rvalue["role"].s());

        auto jwt = jwt_manager->generate_token(role);

        crow::json::wvalue response_data;
        response_data["oid"] = oid;
        response_data["jwt"] = jwt;
        return make_success_response(200, response_data, "Login successful");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_profile_by_oid(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
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

        auto result = db->find_one(Constants::COLLECTION_USERS, filter.view());

        if (!result.has_value()) {
            return make_error_response(401, "Account does not exist");
        }

        auto document_json = bsoncxx::to_json(result.value());
        auto document_rvalue = crow::json::load(document_json);

        crow::json::wvalue document_wvalue;
        document_wvalue["_id"] = document_rvalue["_id"];
        document_wvalue["name"] = document_rvalue["name"];
        document_wvalue["email"] = document_rvalue["email"];
        document_wvalue["role"] = document_rvalue["role"];

        crow::json::wvalue response_data;
        response_data["profile"] = std::move(document_wvalue);
        return make_success_response(200, response_data, "Account retrieved successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}
