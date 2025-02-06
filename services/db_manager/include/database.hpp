#ifndef DATABASE_H
#define DATABASE_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <vector>

class Database {
public:
    Database(const std::string& uri, const std::string& db_name);

    auto find_one(const std::string& collection_name,
                  const bsoncxx::document::view& filter)
        -> bsoncxx::stdx::optional<bsoncxx::document::value>;

    auto insert_one(const std::string& collection_name,
                    const bsoncxx::document::view& document)
        -> bsoncxx::stdx::optional<mongocxx::result::insert_one>;

    auto delete_one(const std::string& collection_name,
                    const bsoncxx::document::view& filter)
        -> bsoncxx::stdx::optional<mongocxx::result::delete_result>;

    auto update_one(const std::string& collection_name,
                    const bsoncxx::document::view& filter,
                    const bsoncxx::document::view& update_document,
                    const bool& upsert = false)
        -> bsoncxx::stdx::optional<mongocxx::result::update>;

    auto find(const std::string& collection_name,
              const bsoncxx::document::view& filter)
        -> mongocxx::cursor;

    auto insert_many(const std::string& collection_name,
                     const std::vector<bsoncxx::document::value>& documents)
        -> bsoncxx::stdx::optional<mongocxx::result::insert_many>;

    auto delete_many(const std::string& collection_name,
                     const bsoncxx::document::view& filter)
        -> bsoncxx::stdx::optional<mongocxx::result::delete_result>;

    auto update_many(const std::string& collection_name,
                     const bsoncxx::document::view& filter,
                     const bsoncxx::document::view& update_document,
                     const bool& upsert = false)
        -> bsoncxx::stdx::optional<mongocxx::result::update>;

private:
    mongocxx::instance instance; 
    mongocxx::client client;
    mongocxx::database db;
};

#endif