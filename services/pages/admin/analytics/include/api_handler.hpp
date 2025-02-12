#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "utils.hpp"

#include "crow.h"

class ApiHandler {
public:
    ApiHandler() = default;

    auto insert_one_post(const crow::request& req, Database& db) -> crow::response;

private:
};

#endif
