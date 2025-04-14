#include "analytics_api_handler.hpp"

#include <bsoncxx/json.hpp>
#include <string>
#include <tuple>

#include "analytics_api_strategy.hpp"
#include "base_api_strategy.hpp"
#include "crow.h"

auto AnalyticsApiHandler::get_one_by_name(const crow::request& req,
                                          std::shared_ptr<DatabaseManager> db_manager,
                                          const std::string& collection_name) -> crow::response {
    return find_one(req, db_manager, collection_name,
                    AnalyticsApiStrategy::process_request_func_get_one_by_name,
                    BaseApiStrategy::process_response_func_get_one);
}

auto AnalyticsApiHandler::get_complaints_statistics(const crow::request& req,
                                                    std::shared_ptr<DatabaseManager> db_manager,
                                                    const std::string& collection_name)
    -> crow::response {
    return aggregate(req, db_manager, collection_name,
                     AnalyticsApiStrategy::process_request_func_get_complaints_statistics,
                     AnalyticsApiStrategy::create_pipeline_func_filter_and_group,
                     AnalyticsApiStrategy::process_response_func_get_complaints_statistics);
}

auto AnalyticsApiHandler::get_complaints_statistics_over_time(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name) -> crow::response {
    return aggregate(
        req, db_manager, collection_name,
        AnalyticsApiStrategy::process_request_func_get_complaints_statistics_over_time,
        AnalyticsApiStrategy::create_pipeline_func_filter_and_group,
        AnalyticsApiStrategy::process_response_func_get_complaints_statistics_over_time);
}

auto AnalyticsApiHandler::get_complaints_statistics_grouped(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name) -> crow::response {
    return aggregate(req, db_manager, collection_name,
                     AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped,
                     AnalyticsApiStrategy::create_pipeline_func_filter_and_group,
                     AnalyticsApiStrategy::process_response_func_get_complaints_statistics_grouped);
}

auto AnalyticsApiHandler::get_complaints_statistics_grouped_over_time(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name) -> crow::response {
    return aggregate(
        req, db_manager, collection_name,
        AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped_over_time,
        AnalyticsApiStrategy::create_pipeline_func_filter_and_group,
        AnalyticsApiStrategy::process_response_func_get_complaints_statistics_grouped_over_time);
}

auto AnalyticsApiHandler::get_complaints_statistics_grouped_by_sentiment_value(
    const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name) -> crow::response {
    return aggregate(
        req, db_manager, collection_name,
        AnalyticsApiStrategy::
            process_request_func_get_complaints_statistics_grouped_by_sentiment_value,
        AnalyticsApiStrategy::create_pipeline_func_filter_and_bucket,
        AnalyticsApiStrategy::
            process_response_func_get_complaints_statistics_grouped_by_sentiment_value);
}