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
    auto _get_complaints_grouped_by_field(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_complaints_grouped_by_field_over_time(std::shared_ptr<Database> db, const std::string& group_by_field, const std::string& time_bucket_regex, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    
    auto _get_complaints_grouped_by_sentiment_value(std::shared_ptr<Database> db, const double& bucket_size, const bsoncxx::document::view& filter) -> mongocxx::cursor;

    auto _get_complaints_sorted_by_fields(std::shared_ptr<Database> db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor;
};

#endif