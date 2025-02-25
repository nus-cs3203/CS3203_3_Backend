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

    auto reddit_perform_update(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response;

private:

};

#endif