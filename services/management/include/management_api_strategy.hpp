#ifndef MANAGEMENT_API_STRATEGY_H
#define MANAGEMENT_API_STRATEGY_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <tuple>
#include <vector>

#include "crow.h"

namespace ManagementApiStrategy {
auto process_request_func_get_all(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value>;
auto process_request_func_get_by_daterange(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value>;
auto process_request_func_get_many(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value>;
auto process_request_func_get_one_by_oid(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find>;
auto process_request_func_get_statistics_poll_responses(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value>;

auto process_request_func_delete_one_by_oid(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>;
auto process_request_func_delete_many_by_oids(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options>;

auto process_request_func_update_one_by_oid(const crow::request& req)
    -> std::tuple<bsoncxx::document::value, bsoncxx::document::value, mongocxx::options::update>;

auto process_response_func_get(mongocxx::cursor& cursor) -> crow::json::wvalue;
auto process_response_func_get_statistics_poll_responses(mongocxx::cursor& cursor)
    -> crow::json::wvalue;
}  // namespace ManagementApiStrategy

#endif