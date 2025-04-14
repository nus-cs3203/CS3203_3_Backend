#ifndef USER_API_HANDLER_H
#define USER_API_HANDLER_H

#include <memory>
#include <string>

#include "crow.h"
#include "database_manager.hpp"
#include "management_api_handler.hpp"

class UserApiHandler : public ManagementApiHandler {
   public:
    auto login(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
               const std::string& collection_name) -> crow::response;

    auto get_one_profile_by_oid(const crow::request& req,
                                std::shared_ptr<DatabaseManager> db_manager,
                                const std::string& collection_name) -> crow::response;

    auto insert_one_account_admin(const crow::request& req,
                                  std::shared_ptr<DatabaseManager> db_manager,
                                  const std::string& collection_name) -> crow::response;

    auto insert_one_account_citizen(const crow::request& req,
                                    std::shared_ptr<DatabaseManager> db_manager,
                                    const std::string& collection_name) -> crow::response;
};

#endif