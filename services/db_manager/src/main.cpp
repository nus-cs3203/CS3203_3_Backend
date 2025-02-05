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

    std::string insert_one(const std::string& collection_name, const bsoncxx::document::view& document) {
        auto collection = db[collection_name];
        auto result = collection.insert_one(document);
        if (result && result->inserted_id().type() == bsoncxx::type::k_oid) {
            std::string id = result->inserted_id().get_oid().value.to_string();
            std::cout << "Inserted to collection " << collection_name << ": " << bsoncxx::to_json(document) << " with id " << id << "." << std::endl;
            return id;
        }
        return "";
    }

    // void delete_document(const std::string& collection_name, const bsoncxx::document::view_or_value& filter) {
    //     auto collection = db_[collection_name];
    //     auto result = collection.delete_one(filter);
    //     if (result && result->deleted_count() > 0) {
    //         std::cout << "Document deleted from '" << collection_name << "'." << std::endl;
    //     } else {
    //         std::cout << "No matching document found to delete." << std::endl;
    //     }
    // }

    bsoncxx::stdx::optional<bsoncxx::document::value> find_one(const std::string& collection_name, const bsoncxx::document::view& filter) {
        auto collection = db[collection_name];
        auto result = collection.find_one(filter);
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

//json to bson converter
bsoncxx::document::value json_to_bson(const crow::json::rvalue& json_document) {
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

            db.insert_one(collection_name, bson_document.view());

            crow::json::wvalue res;
            res["success"] = true;
            res["message"] = "Document inserted successfully";

            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    // GET /find_one
    CROW_ROUTE(app, "/find_one").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (!body || !body.has("collection") || !body.has("filter")) {
            return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
        }

        std::string collection_name = body["collection"].s();
        auto json_filter = body["filter"];

        bsoncxx::document::value bson_filter = json_to_bson(json_filter);
        bsoncxx::stdx::optional<bsoncxx::document::value> result = db.find_one(collection_name, bson_filter.view());

        if (!result.has_value()) {
            return crow::response(200, R"({"success": false, "message": "No matching documents found."})");
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
        res["message"] = "Document was found successfully.";
        return crow::response(200, res);
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