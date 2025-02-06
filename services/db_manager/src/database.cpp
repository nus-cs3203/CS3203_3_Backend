#include "database.hpp"

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

Database::Database(const std::string& uri, const std::string& db_name)
    : instance{}, client{mongocxx::uri{uri}}, db{client[db_name]} {}

auto Database::find_one(const std::string& collection_name, const bsoncxx::document::view& filter)
    -> bsoncxx::stdx::optional<bsoncxx::document::value> {
    auto collection = db[collection_name];
    return collection.find_one(filter);
}

auto Database::insert_one(const std::string& collection_name, const bsoncxx::document::view& document)
    -> bsoncxx::stdx::optional<mongocxx::result::insert_one> {
    auto collection = db[collection_name];
    return collection.insert_one(document);
}

auto Database::delete_one(const std::string& collection_name, const bsoncxx::document::view& filter)
    -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
    auto collection = db[collection_name];
    return collection.delete_one(filter);
}

auto Database::update_one(const std::string& collection_name,
                          const bsoncxx::document::view& filter,
                          const bsoncxx::document::view& update_document,
                          const bool& upsert)
    -> bsoncxx::stdx::optional<mongocxx::result::update> {
    mongocxx::options::update options;
    options.upsert(upsert);
    auto collection = db[collection_name];
    return collection.update_one(filter, update_document, options);
}

auto Database::find(const std::string& collection_name, const bsoncxx::document::view& filter)
    -> mongocxx::cursor {
    auto collection = db[collection_name];
    return collection.find(filter);
}

auto Database::insert_many(const std::string& collection_name,
                           const std::vector<bsoncxx::document::value>& documents)
    -> bsoncxx::stdx::optional<mongocxx::result::insert_many> {
    auto collection = db[collection_name];
    return collection.insert_many(documents);
}

auto Database::delete_many(const std::string& collection_name,
                           const bsoncxx::document::view& filter)
    -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
    auto collection = db[collection_name];
    return collection.delete_many(filter);
}

auto Database::update_many(const std::string& collection_name,
                           const bsoncxx::document::view& filter,
                           const bsoncxx::document::view& update_document,
                           const bool& upsert)
    -> bsoncxx::stdx::optional<mongocxx::result::update> {
    mongocxx::options::update options;
    options.upsert(upsert);
    auto collection = db[collection_name];
    return collection.update_many(filter, update_document, options);
}
