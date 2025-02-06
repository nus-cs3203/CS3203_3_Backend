#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "utils.hpp"

#include "crow.h"

class ApiHandler {
public:
    ApiHandler() = default;

    auto insert_one(const crow::request& req, Database& db) -> crow::response;

    auto insert_many(const crow::request& req, Database& db) -> crow::response;

    auto find_one(const crow::request& req, Database& db) -> crow::response;

    auto find(const crow::request& req, Database& db) -> crow::response;

    auto delete_one(const crow::request& req, Database& db) -> crow::response;

    auto delete_many(const crow::request& req, Database& db) -> crow::response;

    auto update_one(const crow::request& req, Database& db) -> crow::response;

    auto update_many(const crow::request& req, Database& db) -> crow::response;

private:
};

#endif
