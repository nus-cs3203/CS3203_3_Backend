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

int main(int argc, char* argv[]) {
    const std::string uri = argc > 1 ? argv[1] : "mongodb://127.0.0.1:27017";
    const std::string db_name = argc > 2 ? argv[2] : "CS3203";
    Database db(uri, db_name);
    
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::Warning);

    // GET /find_one
    CROW_ROUTE(app, "/find_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!body || !body.has("collection") || !body.has("filter")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];

            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            bsoncxx::stdx::optional<bsoncxx::document::value> result = db.find_one(collection_name, bson_filter.view());

            if (!result.has_value()) {
                return crow::response(200, R"({"success": true, "document": null, "message": "No matching documents found"})");
            }

            std::cout << "Found document: " << bsoncxx::to_json(result.value()) << std::endl;

            std::string document_json = bsoncxx::to_json(result.value());
            auto document_wvalue = crow::json::load(document_json);
            if (!document_wvalue) {
                return crow::response(500, R"({"success": false, "message": "Error converting document"})");
            }

            crow::json::wvalue res;
            res["success"] = true;
            res["document"] = document_wvalue;
            res["message"] = "Document was found successfully";
            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    // POST /insert_one
    CROW_ROUTE(app, "/insert_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!body || !body.has("collection") || !body.has("document")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_document = body["document"];
            bsoncxx::document::value bson_document = json_to_bson(json_document);

            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db.insert_one(collection_name, bson_document.view());

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to insert document: no result was returned from the database operation"})");
            }

            std::string id = result->inserted_id().get_oid().value.to_string();
            std::cout << "Inserted to collection " << collection_name << ": " << json_document << " with id " << id << "." << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["_id"] = id;
            res["message"] = "Document inserted successfully";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/delete_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!body || !body.has("collection") || !body.has("filter")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];

            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            bsoncxx::stdx::optional<mongocxx::result::delete_result> result = db.delete_one(collection_name, bson_filter.view());

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to delete document: no result was returned from the database operation"})");
            }

            int delete_count = result->deleted_count();
            std::cout << "Deleted " << delete_count << " documents from collection " << collection_name << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["count"] = delete_count;
            res["message"] = "Document deleted successfully";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/update_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!body || !body.has("collection") || !body.has("filter") || !body.has("update_document")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            auto json_update_document = body["update_document"];
            bsoncxx::document::value bson_update_document = json_to_bson(json_update_document);
            bool upsert = false;
            if (body.has("upsert") && body["upsert"]) {
                upsert = true;
            }

            bsoncxx::stdx::optional<mongocxx::result::update> result = db.update_one(collection_name, bson_filter.view(), bson_update_document.view(), upsert);

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to insert document: no result was returned from the database operation"})");
            }

            std::cout << "Updated collection " << collection_name << ": (matched_count, modified_count, upserted_count) is (" << result->matched_count() << ", " << result->modified_count() <<", " << result->upserted_count() << ")" << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["matched_count"] = result->matched_count();
            res["modified_count"] = result->modified_count();
            res["upserted_count"] = result->upserted_count();
            res["message"] = "Update successful";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/find").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!body || !body.has("collection") || !body.has("filter")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];

            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            mongocxx::cursor cursor = db.find(collection_name, bson_filter.view());

            crow::json::wvalue res;
            res["status"] = true;
            res["message"] = "Documents retrieved successfully";

            std::vector<crow::json::wvalue> documents;
            for (auto&& document : cursor) {
                documents.push_back(crow::json::load(bsoncxx::to_json(document)));
            }
            res["documents"] = std::move(documents);

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/insert_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!body || !body.has("collection") || !body.has("documents")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_documents = body["documents"];

            std::vector<bsoncxx::document::value> bson_documents;
            for (auto &document: json_documents) {
                bson_documents.push_back(json_to_bson(document));
            }

            bsoncxx::stdx::optional<mongocxx::result::insert_many> result = db.insert_many(collection_name, bson_documents);

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to insert document: no result was returned from the database operation"})");
            }

            std::cout << "Inserted to collection " << collection_name << ": " << result->inserted_count() << " documents" << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["message"] = "Document inserted successfully";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/delete_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);

            if (!body || !body.has("collection") || !body.has("filter")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];

            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            bsoncxx::stdx::optional<mongocxx::result::delete_result> result = db.delete_many(collection_name, bson_filter.view());

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to delete document: no result was returned from the database operation"})");
            }

            int delete_count = result->deleted_count();
            std::cout << "Deleted " << delete_count << " documents from collection " << collection_name << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["count"] = delete_count;
            res["message"] = "Document deleted successfully";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    CROW_ROUTE(app, "/update_many").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!body || !body.has("collection") || !body.has("filter") || !body.has("update_document")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection_name = body["collection"].s();
            auto json_filter = body["filter"];
            bsoncxx::document::value bson_filter = json_to_bson(json_filter);
            auto json_update_document = body["update_document"];
            bsoncxx::document::value bson_update_document = json_to_bson(json_update_document);
            bool upsert = false;
            if (body.has("upsert") && body["upsert"]) {
                upsert = true;
            }

            bsoncxx::stdx::optional<mongocxx::result::update> result = db.update_many(collection_name, bson_filter.view(), bson_update_document.view(), upsert);

            if (!result) {
                return crow::response(500, R"({"success": false, "message": "Failed to insert document: no result was returned from the database operation"})");
            }

            std::cout << "Updated collection " << collection_name << ": (matched_count, modified_count, upserted_count) is (" << result->matched_count() << ", " << result->modified_count() <<", " << result->upserted_count() << ")" << std::endl;

            crow::json::wvalue res;
            res["success"] = true;
            res["matched_count"] = result->matched_count();
            res["modified_count"] = result->modified_count();
            res["upserted_count"] = result->upserted_count();
            res["message"] = "Update successful";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    app.port(8081).multithreaded().run();
    return 0;
}