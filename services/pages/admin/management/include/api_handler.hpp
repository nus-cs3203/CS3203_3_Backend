#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"
#include <memory>
#include <string>
#include <vector>

class ApiHandler {
public:
    ApiHandler() = default;

    auto get_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    auto delete_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    auto delete_many_by_oids(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    auto update_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;


};

#endif