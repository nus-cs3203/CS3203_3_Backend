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

    auto get_complaints_grouped_by_field(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;
    auto get_complaints_grouped_by_field_over_time(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_grouped_by_sentiment_value(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;

    auto get_complaints_sorted_by_fields(const crow::request& req, std::shared_ptr<Database> db) -> crow::response;
private:
    auto _get_complaints_grouped_by_field_get_cursor(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_field_read_cursor(mongocxx::cursor& cursor, const std::vector<std::string>& categories) -> crow::json::wvalue;
    
    auto _get_complaints_grouped_by_sentiment_value_get_cursor(std::shared_ptr<Database> db, const double& bucket_size, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_sentiment_value_read_cursor(mongocxx::cursor& cursor, const double& bucket_size) -> crow::json::wvalue;

    auto _get_complaints_sorted_by_fields(std::shared_ptr<Database> db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor;

    auto _get_all_categories(std::shared_ptr<Database> db) -> std::vector<std::string>;
    auto _get_months_range(std::chrono::system_clock::time_point start_tp, std::chrono::system_clock::time_point end_tp) -> std::vector<std::string>;
};

#endif