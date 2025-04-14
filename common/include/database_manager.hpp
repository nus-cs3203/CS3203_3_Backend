#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <vector>

#include "crow.h"
#include "env_manager.hpp"

class DatabaseManager {
   public:
    DatabaseManager(const std::string& uri, const std::string& db_name);

    static std::shared_ptr<DatabaseManager> create_from_env(EnvManager env_manager = EnvManager());

    auto find_one(const std::string& collection_name, const bsoncxx::document::view& filter = {},
                  const mongocxx::options::find& option = {})
        -> bsoncxx::stdx::optional<bsoncxx::document::value>;

    auto find(const std::string& collection_name, const bsoncxx::document::view& filter = {},
              const mongocxx::options::find& option = {}) -> mongocxx::cursor;

    auto insert_one(const std::string& collection_name, const bsoncxx::document::view& document,
                    const mongocxx::options::insert& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::insert_one>;

    auto insert_many(const std::string& collection_name,
                     const std::vector<bsoncxx::document::value>& documents,
                     const mongocxx::options::insert& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::insert_many>;

    auto delete_one(const std::string& collection_name, const bsoncxx::document::view& filter,
                    const mongocxx::options::delete_options& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::delete_result>;

    auto delete_many(const std::string& collection_name, const bsoncxx::document::view& filter,
                     const mongocxx::options::delete_options& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::delete_result>;

    auto update_one(const std::string& collection_name, const bsoncxx::document::view& filter,
                    const bsoncxx::document::view& update_document,
                    const mongocxx::options::update& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::update>;

    auto update_many(const std::string& collection_name, const bsoncxx::document::view& filter,
                     const bsoncxx::document::view& update_document,
                     const mongocxx::options::update& option = {})
        -> bsoncxx::stdx::optional<mongocxx::result::update>;

    auto count_documents(const std::string& collection_name, const bsoncxx::document::view& filter,
                         const mongocxx::options::count& option = {}) -> long long int;

    auto aggregate(const std::string& collection_name, const mongocxx::pipeline& pipeline,
                   const mongocxx::options::aggregate& option = {}) -> mongocxx::cursor;

   private:
    static mongocxx::instance instance;
    mongocxx::client client;
    mongocxx::database db;
};

#endif
