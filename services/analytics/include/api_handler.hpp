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

    auto get_category_analytics_by_name(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_statistics(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_statistics_over_time(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_sorted_by_fields(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_grouped_by_field(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_grouped_by_field_over_time(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_grouped_by_sentiment_value(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

private:
    auto _create_filter_complaints(const crow::json::rvalue& json) -> bsoncxx::document::value;
    
    auto _create_aggregate_pipeline_complaints_statistics(const bsoncxx::document::view& filter) -> mongocxx::pipeline;
    auto _read_cursor_complaints_statistics(mongocxx::cursor& cursor) -> crow::json::wvalue;

    auto _create_aggregate_pipeline_complaints_statistics_over_time(const bsoncxx::document::view& filter) -> mongocxx::pipeline;
    auto _read_cursor_complaints_statistics_over_time(mongocxx::cursor& cursor) -> std::vector<crow::json::wvalue>;

    auto _create_month_range(const int& start_month, const int& start_year, const int& end_month, const int& end_year) -> std::vector<std::pair<int, int>>;
    auto _remove_irrelevant_month_year_complaints_statistics_over_time(std::vector<crow::json::wvalue>& documents, const std::vector<std::pair<int, int>>& month_range) -> std::vector<crow::json::wvalue>;
    auto _fill_missing_month_year_complaints_statistics_over_time(std::vector<crow::json::wvalue>& documents, const std::vector<std::pair<int, int>>& month_range) -> std::vector<crow::json::wvalue>;

    auto _create_find_option_complaints_sorted_by_fields(bsoncxx::builder::basic::document& sort_builder, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::options::find;
    auto _read_cursor_complaints_sorted_by_fields(mongocxx::cursor& cursor) -> std::vector<crow::json::wvalue>;

    auto _get_complaints_grouped_by_field_get_cursor(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_field_read_cursor(mongocxx::cursor& cursor, const std::vector<std::string>& categories) -> crow::json::wvalue;
    
    auto _get_complaints_grouped_by_field_over_time_get_cursor(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_field_over_time_read_cursor(mongocxx::cursor& cursor, const std::vector<std::string>& categories, const std::string& group_by_field, const long long int& start_date_ts, const long long int& end_date_ts) -> crow::json::wvalue::list;

    auto _get_complaints_grouped_by_sentiment_value_get_cursor(std::shared_ptr<Database> db, const double& bucket_size, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_sentiment_value_read_cursor(mongocxx::cursor& cursor, const double& bucket_size) -> crow::json::wvalue;

    auto _get_group_by_field_all_distinct_values(std::shared_ptr<Database> db, const std::string& group_by_field) -> std::vector<std::string>;
    auto _get_collection_name_from_group_by_field(const std::string& group_by_field) -> std::string;
    auto _get_months_range(std::chrono::system_clock::time_point start_tp, std::chrono::system_clock::time_point end_tp) -> std::vector<std::string>;
};

#endif