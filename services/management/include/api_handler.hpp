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

    auto get_all(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    auto get_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name, const bool& should_convert_date = false) -> crow::response;
    auto search(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const bool& should_convert_date = false) -> crow::response;
    
    auto insert_one(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;

    auto delete_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    auto delete_many_by_oids(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;
    
    auto update_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name, const bool& should_convert_date = false) -> crow::response;

};

#endif