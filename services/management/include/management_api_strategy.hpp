#ifndef MANAGEMENT_API_STRATEGY_H
#define MANAGEMENT_API_STRATEGY_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <tuple>

namespace ManagementApiStrategy {
    auto process_request_func_get_one_by_oid(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find>; 
    auto process_response_func_get_one_by_oid(const bsoncxx::document::value& doc) -> crow::json::wvalue;
}

#endif