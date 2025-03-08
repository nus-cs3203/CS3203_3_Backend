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

    auto signup(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;
    auto login(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

};

#endif