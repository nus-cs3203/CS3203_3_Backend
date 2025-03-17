#include "constants.hpp"
#include "database_manager.hpp"
#include "env_manager.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <vector>

DatabaseManager::DatabaseManager(
    const std::string& uri, 
    const std::string& db_name
) : instance{}, client{mongocxx::uri{uri}}, db{client[db_name]} {}

auto DatabaseManager::create_from_env(EnvManager env_manager) -> DatabaseManager {
    auto MONGO_URI = env_manager.read_env("MONGO_URI", Constants::MONGO_URI);
    auto DB_NAME = env_manager.read_env("DB_NAME", Constants::DB_NAME);
    return DatabaseManager(MONGO_URI, DB_NAME);
}

auto DatabaseManager::find_one(const std::string& collection_name, const bsoncxx::document::view& filter, const mongocxx::options::find& option)
    -> bsoncxx::stdx::optional<bsoncxx::document::value> {
    auto collection = db[collection_name];
    return collection.find_one(filter, option);
}

auto DatabaseManager::find(const std::string& collection_name, const bsoncxx::document::view& filter, const mongocxx::options::find& option)
    -> mongocxx::cursor {
    auto collection = db[collection_name];
    return collection.find(filter, option);
}

auto DatabaseManager::insert_one(const std::string& collection_name, const bsoncxx::document::view& document, const mongocxx::options::insert& option)
    -> bsoncxx::stdx::optional<mongocxx::result::insert_one> {
    auto collection = db[collection_name];
    return collection.insert_one(document, option);
}

auto DatabaseManager::insert_many(const std::string& collection_name, const std::vector<bsoncxx::document::value>& documents, const mongocxx::options::insert& option)
    -> bsoncxx::stdx::optional<mongocxx::result::insert_many> {
    auto collection = db[collection_name];
    return collection.insert_many(documents, option);
}

auto DatabaseManager::delete_one(const std::string& collection_name, const bsoncxx::document::view& filter, const mongocxx::options::delete_options& option)
    -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
    auto collection = db[collection_name];
    return collection.delete_one(filter, option);
}

auto DatabaseManager::delete_many(const std::string& collection_name, const bsoncxx::document::view& filter, const mongocxx::options::delete_options& option)
    -> bsoncxx::stdx::optional<mongocxx::result::delete_result> {
    auto collection = db[collection_name];
    return collection.delete_many(filter, option);
}

auto DatabaseManager::update_one(
        const std::string& collection_name,
        const bsoncxx::document::view& filter,
        const bsoncxx::document::view& update_document,
        const mongocxx::options::update& option)
    -> bsoncxx::stdx::optional<mongocxx::result::update> {
    auto collection = db[collection_name];
    return collection.update_one(filter, update_document, option);
}

auto DatabaseManager::update_many(
        const std::string& collection_name,
        const bsoncxx::document::view& filter,
        const bsoncxx::document::view& update_document,
        const mongocxx::options::update& option)
    -> bsoncxx::stdx::optional<mongocxx::result::update> {
    auto collection = db[collection_name];
    return collection.update_many(filter, update_document, option);
}

auto DatabaseManager::count_documents(const std::string& collection_name, const bsoncxx::document::view& filter, const mongocxx::options::count& option) -> long long int {
    auto collection = db[collection_name];
    return collection.count_documents(filter, option);
}

auto DatabaseManager::aggregate(const std::string& collection_name, const mongocxx::pipeline& pipeline, const mongocxx::options::aggregate& option) -> mongocxx::cursor {
    auto collection = db[collection_name];
    return collection.aggregate(pipeline, option);
}
