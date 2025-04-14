#ifndef ANALYTICS_API_HANDLER_H
#define ANALYTICS_API_HANDLER_H

#include <memory>
#include <string>

#include "base_api_handler.hpp"
#include "crow.h"
#include "database_manager.hpp"

class AnalyticsApiHandler : public BaseApiHandler {
   public:
    auto get_one_by_name(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                         const std::string& collection_name) -> crow::response;

    auto get_complaints_statistics(const crow::request& req,
                                   std::shared_ptr<DatabaseManager> db_manager,
                                   const std::string& collection_name) -> crow::response;

    auto get_complaints_statistics_over_time(const crow::request& req,
                                             std::shared_ptr<DatabaseManager> db_manager,
                                             const std::string& collection_name) -> crow::response;

    auto get_complaints_statistics_grouped(const crow::request& req,
                                           std::shared_ptr<DatabaseManager> db_manager,
                                           const std::string& collection_name) -> crow::response;

    auto get_complaints_statistics_grouped_over_time(const crow::request& req,
                                                     std::shared_ptr<DatabaseManager> db_manager,
                                                     const std::string& collection_name)
        -> crow::response;

    auto get_complaints_statistics_grouped_by_sentiment_value(
        const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
        const std::string& collection_name) -> crow::response;

   private:
};

#endif