#include "database.hpp"
#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>

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

int main(int argc, char* argv[]) {
    const std::string uri = argc > 1 ? argv[1] : "mongodb://127.0.0.1:27017";
    const std::string db_name = argc > 2 ? argv[2] : "CS3203";
    Database db(uri, db_name);
    
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::Warning);

    // POST /insert_one
    CROW_ROUTE(app, "/insert_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!validate_request(body, {"collection", "document"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_document = body["document"];
            auto bson_document = json_to_bson(json_document);

            auto result = db.insert_one(collection_name, bson_document.view());

            auto id = result->inserted_id().get_oid().value.to_string();
            std::cout << "Inserted to collection " << collection_name << ": " << json_document << " with id " << id << "." << std::endl;

            crow::json::wvalue response_data;
            response_data["_id"] = id;
            return make_success_response(200, response_data, "Document(s) inserted successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/insert_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  

            if (!validate_request(body, {"collection", "documents"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_documents = body["documents"];
            std::vector<bsoncxx::document::value> bson_documents;
            for (auto &document: json_documents) {
                bson_documents.push_back(json_to_bson(document));
            }

            auto result = db.insert_many(collection_name, bson_documents);

            int inserted_count = result->inserted_count();
            std::cout << "Inserted to collection " << collection_name << ": " << inserted_count << " documents" << std::endl;

            crow::json::wvalue response_data;
            response_data["inserted_count"] = inserted_count;
            return make_success_response(200, response_data, "Document(s) inserted successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    // GET /find_one
    CROW_ROUTE(app, "/find_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!validate_request(body, {"collection", "filter"})) {
                return make_error_response(400, "Invalid request format"); 
            }

            auto collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);

            auto result = db.find_one(collection_name, bson_filter.view());

            if (!result.has_value()) {
                return make_success_response(200, {}, "No matching documents found");
            }

            std::cout << "Found document: " << bsoncxx::to_json(result.value()) << std::endl;

            auto document_json = bsoncxx::to_json(result.value());
            auto document_wvalue = crow::json::load(document_json);

            crow::json::wvalue response_data;
            response_data["document"] = document_wvalue;
            return make_success_response(200, response_data, "Document found successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/find").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!validate_request(body, {"collection", "filter"})) {
                return make_error_response(400, "Invalid request format");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);

            auto cursor = db.find(collection_name, bson_filter.view());
            std::vector<crow::json::wvalue> documents;

            for (auto&& document : cursor) {
                auto document_json = bsoncxx::to_json(document);
                documents.push_back(crow::json::load(document_json));
            }

            crow::json::wvalue response_data;
            response_data["documents"] = std::move(documents);
            return make_success_response(200, response_data, "Documents retrieved successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/delete_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!validate_request(body, {"collection", "filter"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);

            auto result = db.delete_one(collection_name, bson_filter.view());

            int deleted_count = result->deleted_count();
            std::cout << "Deleted " << deleted_count << " documents from collection " << collection_name << std::endl;

            crow::json::wvalue response_data;
            response_data["deleted_count"] = deleted_count;
            return make_success_response(200, response_data, "Document(s) deleted successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/delete_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!validate_request(body, {"collection", "filter"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);

            auto result = db.delete_many(collection_name, bson_filter.view());

            int deleted_count = result->deleted_count();
            std::cout << "Deleted " << deleted_count << " documents from collection " << collection_name << std::endl;

            crow::json::wvalue response_data;
            response_data["deleted_count"] = deleted_count;
            return make_success_response(200, response_data, "Document(s) deleted successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });


    CROW_ROUTE(app, "/update_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!validate_request(body, {"collection", "filter", "update_document", "upsert"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);
            auto json_update_document = body["update_document"];
            auto bson_update_document = json_to_bson(json_update_document);
            bool upsert = body["upsert"].b();

            auto result = db.update_one(collection_name, bson_filter.view(), bson_update_document.view(), upsert);

            int matched_count = result->matched_count(), modified_count = result->modified_count(), upserted_count = result->upserted_count();
            std::cout << "Updated collection " << collection_name << ": (matched_count, modified_count, upserted_count) is (" << matched_count << ", " << modified_count << ", " << upserted_count << ")" << std::endl;

            crow::json::wvalue response_data;
            response_data["matched_count"] = matched_count;
            response_data["modified_count"] = modified_count;
            response_data["upserted_count"] = upserted_count;
            return make_success_response(200, response_data, "Document(s) updated successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/update_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!validate_request(body, {"collection", "filter", "update_document", "upsert"})) {
                return make_error_response(400, "Invalid request format");
            }

            auto collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            auto bson_filter = json_to_bson(json_filter);
            auto json_update_document = body["update_document"];
            auto bson_update_document = json_to_bson(json_update_document);
            bool upsert = body["upsert"].b();

            auto result = db.update_many(collection_name, bson_filter.view(), bson_update_document.view(), upsert);

            int matched_count = result->matched_count(), modified_count = result->modified_count(), upserted_count = result->upserted_count();
            std::cout << "Updated collection " << collection_name << ": (matched_count, modified_count, upserted_count) is (" << matched_count << ", " << modified_count << ", " << upserted_count << ")" << std::endl;

            crow::json::wvalue response_data;
            response_data["matched_count"] = matched_count;
            response_data["modified_count"] = modified_count;
            response_data["upserted_count"] = upserted_count;
            return make_success_response(200, response_data, "Document(s) updated successfully");
        } catch (const std::exception& e) {
            return make_error_response(500, std::string("Server error: ") + e.what());
        }
    });

    app.port(8081).multithreaded().run();
    return 0;
}