#ifndef ANALYTICS_API_STRATEGY_H
#define ANALYTICS_API_STRATEGY_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

namespace AnalyticsApiStrategy {
    auto process_request_func_get_one_by_name(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find>; 
    auto process_request_func_get_complaints_statistics(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate>;
    auto process_request_func_get_complaints_statistics_over_time(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate>;
    auto process_request_func_get_complaints_statistics_grouped(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate>;
    auto process_request_func_get_complaints_statistics_grouped_over_time(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate>;

    auto create_pipeline_func_get_complaints_statistics(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline;
    auto create_pipeline_func_get_complaints_statistics_over_time(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline;
    auto create_pipeline_func_get_complaints_statistics_grouped(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline;
    auto create_pipeline_func_get_complaints_statistics_grouped_over_time(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline;

    auto process_response_func_get_complaints_statistics(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue;
    auto process_response_func_get_complaints_statistics_over_time(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue;
    auto process_response_func_get_complaints_statistics_grouped(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue;
    auto process_response_func_get_complaints_statistics_grouped_over_time(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue;

    auto _create_month_range(const std::string& start_date, const std::string& end_date) -> std::vector<std::pair<int, int>>;

    extern std::unordered_map<std::string, std::vector<std::string>> GROUP_BY_FIELD_VALUES_MAPPER;
}

#endif