#ifndef MANAGEMENT_API_HANDLER_H
#define MANAGEMENT_API_HANDLER_H

#include <memory>
#include <string>

#include "base_api_handler.hpp"
#include "crow.h"
#include "database_manager.hpp"

class ManagementApiHandler : public BaseApiHandler {
   public:
    auto get_one_by_oid(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                        const std::string& collection_name) -> crow::response;

    auto get_all(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                 const std::string& collection_name) -> crow::response;

    auto get_by_daterange(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                          const std::string& collection_name) -> crow::response;

    auto get_many(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                  const std::string& collection_name) -> crow::response;

    auto get_statistics_poll_responses(const crow::request& req,
                                       std::shared_ptr<DatabaseManager> db_manager,
                                       const std::string& collection_name) -> crow::response;

    auto delete_one_by_oid(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                           const std::string& collection_name) -> crow::response;

    auto delete_many_by_oids(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                             const std::string& collection_name) -> crow::response;

    auto update_one_by_oid(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                           const std::string& collection_name) -> crow::response;

   private:
};

#endif