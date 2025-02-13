#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "database.hpp"
#include "utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <vector>

class ApiHandler {
public:
    ApiHandler() = default;

    auto get_sentiment_analytics_by_source_over_time(const crow::request& req, Database& db) -> crow::response;
    auto get_sentiment_analytics_by_category_over_time(const crow::request& req, Database& db) -> crow::response;
    auto get_sentiment_analytics_by_value(const crow::request& req, Database& db) -> crow::response;
    auto get_posts_grouped(const crow::request& req, Database& db) -> crow::response;
    auto get_posts_sorted(const crow::request& req, Database& db) -> crow::response;
private:
    auto _get_posts_grouped(Database& db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor;
    auto _get_posts_sorted(Database& db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor;
};

#endif
