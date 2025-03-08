#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "jwt_manager.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"
#include <memory>
#include <string>
#include <vector>

class ApiHandler {
public:
    ApiHandler() = default;

    auto signup(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;
    auto login(const crow::request& req, std::shared_ptr<Database> db, std::shared_ptr<JwtManager> jwt_manager) -> crow::response;
    auto get_profile_by_oid(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

};

#endif