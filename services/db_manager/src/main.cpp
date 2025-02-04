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
        : instance_{}, client_{mongocxx::uri{uri}}, db_{client_[db_name]} {}

    void create_collection(const std::string& collection_name) {
        db_.create_collection(collection_name);
        std::cout << "Collection '" << collection_name << "' created successfully." << std::endl;
    }

    void insert_document(const std::string& collection_name, const bsoncxx::document::view_or_value& doc) {
        auto collection = db_[collection_name];
        collection.insert_one(doc);
        std::cout << "Document inserted into '" << collection_name << "'." << std::endl;
    }

    void delete_document(const std::string& collection_name, const bsoncxx::document::view_or_value& filter) {
        auto collection = db_[collection_name];
        auto result = collection.delete_one(filter);
        if (result && result->deleted_count() > 0) {
            std::cout << "Document deleted from '" << collection_name << "'." << std::endl;
        } else {
            std::cout << "No matching document found to delete." << std::endl;
        }
    }

    void find_document(const std::string& collection_name, const bsoncxx::document::view_or_value& filter) {
        auto collection = db_[collection_name];
        auto result = collection.find_one(filter);
        if (result) {
            std::cout << "Found document: " << bsoncxx::to_json(*result) << std::endl;
        } else {
            std::cout << "No matching document found." << std::endl;
        }
    }

private:
    mongocxx::instance instance_;
    mongocxx::client client_;
    mongocxx::database db_;
};

//json to bson converter
bsoncxx::document::value json_to_bson(const crow::json::rvalue& json_doc) {
    bsoncxx::builder::basic::document bson_doc;
    for (const auto& element : json_doc) {
        std::string key = element.key();
        const crow::json::rvalue& value = element;

        if (value.t() == crow::json::type::String) {
            bson_doc.append(kvp(key, value.s()));
        } else if (value.t() == crow::json::type::Number) {
            bson_doc.append(kvp(key, value.d()));  
        } else if (value.t() == crow::json::type::True or value.t() == crow::json::type::False) {  
            bson_doc.append(kvp(key, value.b()));
        } else if (value.t() == crow::json::type::List) {
            bsoncxx::builder::basic::array bson_array;
            for (const auto& list_element : value) {
                if (list_element.t() == crow::json::type::String) {
                    bson_array.append(list_element.s());
                } else if (list_element.t() == crow::json::type::Number) {
                    bson_array.append(list_element.d());
                } else if (list_element.t() == crow::json::type::True or list_element.t() == crow::json::type::False) {  
                    bson_array.append(list_element.b());
                }
            }
            bson_doc.append(kvp(key, bson_array));
        } else if (value.t() == crow::json::type::Object) {
            bson_doc.append(kvp(key, json_to_bson(value).view()));  // Recursively handle nested objects
        }
    }
    return bson_doc.extract();
}

int main() {
    const std::string uri = "mongodb://127.0.0.1:27017";
    Database db(uri, "CS3203");
    
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::Warning);

    // POST /insert Endpoint
    CROW_ROUTE(app, "/insert").methods(crow::HTTPMethod::Post)
    ([&db](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);  // Parse JSON request body

            if (!body || !body.has("collection") || !body.has("document")) {
                return crow::response(400, R"({"success": false, "message": "Invalid request format"})");
            }

            std::string collection = body["collection"].s();
            auto json_doc = body["document"];

            // Convert Crow JSON to BSON
            bsoncxx::document::value bson_doc = json_to_bson(json_doc);

            // Insert into MongoDB
            db.insert_document(collection, bson_doc.view());

            // Return success response
            crow::json::wvalue res;
            res["success"] = true;
            res["message"] = "Document inserted successfully";
            return crow::response(200, res);
        } catch (const std::exception& e) {
            return crow::response(500, R"({"success": false, "message": ")" + std::string(e.what()) + R"("})");
        }
    });

    app.port(8081).multithreaded().run();
    return 0;
}