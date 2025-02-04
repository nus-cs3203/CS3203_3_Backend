#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

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

int main() {
    const std::string uri = "mongodb://127.0.0.1:27017";
    Database db(uri, "sample_mflix");
    
    db.create_collection("movies");

    db.insert_document("movies", make_document(kvp("title", "The Shawshank Redemption"), kvp("year", 1994)));
    
    db.find_document("movies", make_document(kvp("title", "The Shawshank Redemption")));
    
    db.delete_document("movies", make_document(kvp("title", "The Shawshank Redemption")));
    
    return 0;
}
