#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include "crow.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class Database {
public:
    Database(const std::string& uri, const std::string& db_name)
        : instance{}, client{mongocxx::uri{uri}}, db{client[db_name]} {}

    auto find_one(const std::string& collection_name, const bsoncxx::document::view& filter) -> bsoncxx::stdx::optional<bsoncxx::document::value> {
        auto collection = db[collection_name];
        auto result = collection.find_one(filter);
        return result;
    }

    auto insert_one(const std::string& collection_name, const bsoncxx::document::view& document) -> bsoncxx::stdx::optional<mongocxx::result::insert_one> {
        auto collection = db[collection_name];
        auto result = collection.insert_one(document);
        return result;
    }

    auto delete_one(const std::string& collection_name, const bsoncxx::document::view& filter) -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
        auto collection = db[collection_name];
        auto result = collection.delete_one(filter);
        return result;
    }

    auto find(const std::string& collection_name, const bsoncxx::document::view& filter) -> mongocxx::cursor {
        auto collection = db[collection_name];
        auto cursor = collection.find(filter);
        return cursor;
    }

    auto insert_many(const std::string& collection_name, const std::vector<bsoncxx::document::value>& documents) -> bsoncxx::stdx::optional<mongocxx::result::insert_many> {
        auto collection = db[collection_name];
        auto result = collection.insert_many(documents);
        return result;
    }

    auto delete_many(const std::string& collection_name, const bsoncxx::document::view& filter) -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
        auto collection = db[collection_name];
        auto result = collection.delete_many(filter);
        return result;
    }

    // std::vector<std::string> find_documents(const std::string& collection_name, const bsoncxx::document::view_or_value& filter, int limit) {
    //     std::vector<std::string> results;
    //     auto collection = db_[collection_name];

    //     mongocxx::options::find find_options;
    //     find_options.limit(limit);  // Set limit on number of documents

    //     auto cursor = collection.find(filter, find_options);

    //     for (auto&& doc : cursor) {
    //         results.push_back(bsoncxx::to_json(doc));  // Convert BSON to JSON string
    //     }

    //     if (results.empty()) {
    //         std::cout << "No matching documents found." << std::endl;
    //     }

    //     return results;
    // }

private:
    mongocxx::instance instance;
    mongocxx::client client;
    mongocxx::database db;
};

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value {
    std::ostringstream oss;
    oss << json_document;
    std::string json_str = oss.str();
    return bsoncxx::from_json(json_str);
}

int main() {
    const std::string uri = "mongodb://127.0.0.1:27017";
    Database db(uri, "CS3203");
    
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


    // // GET /find_documents Endpoint
    // CROW_ROUTE(app, "/find_documents").methods(crow::HTTPMethod::Post)
    // ([&db](const crow::request& req) {
    //     auto body = crow::json::load(req.body);

    //     if (!body || !body.has("collection") || !body.has("filter") || !body.has("limit")) {
    //         return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
    //     }

    //     std::string collection = body["collection"].s();
    //     int limit = body["limit"].i();
    //     auto json_filter = body["filter"];

    //     bsoncxx::document::value filter_bson = json_to_bson(json_filter);
    //     std::vector<std::string> documents = db.find_documents(collection, filter_bson.view(), limit);

    //     crow::json::wvalue res;
    //     res["success"] = true;
    //     res["documents"] = crow::json::wvalue::list(documents.begin(), documents.end());
    //     return crow::response(200, res);
    // });



    app.port(8081).multithreaded().run();
    return 0;
}