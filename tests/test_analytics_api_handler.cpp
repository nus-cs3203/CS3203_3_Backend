#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <cstdlib>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "analytics_api_handler.hpp"
#include "crow.h"
#include "database_manager.hpp"

// For convenience.
using bsoncxx::to_json;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Helper to clear all documents in a collection.
static inline void cleanup_collection(DatabaseManager& dbManager,
                                      const std::string& collection_name) {
    dbManager.delete_many(collection_name, make_document().view());
}

// -------- Test for get_one_by_name --------
TEST(AnalyticsApiHandlerTest, GetOneByName) {
    // Setup a test database and collection.
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_get_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a dummy document that should be retrieved.
    auto doc = make_document(kvp("name", "Housing"), kvp("summary", "Test summary"));
    auto insert_result = db_ptr->insert_one(collection, doc.view());
    ASSERT_TRUE(insert_result.has_value());

    // Build a request with the name.
    crow::request req;
    req.body = "{\"name\": \"Housing\"}";

    auto response = handler.get_one_by_name(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    // Verify that the response contains a success message (adjust to your real response message).
    EXPECT_NE(response.body.find("Server processed get request successfully"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_complaints_statistics --------
TEST(AnalyticsApiHandlerTest, GetComplaintsStatistics) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_stats";
    cleanup_collection(*db_ptr, collection);

    // Insert dummy complaints documents.
    db_ptr->insert_one(collection,
                       make_document(kvp("category", "Housing"), kvp("sentiment", -0.8)).view());
    db_ptr->insert_one(collection,
                       make_document(kvp("category", "Housing"), kvp("sentiment", -0.82)).view());

    // Build a request with an empty filter (or add further fields as required).
    crow::request req;
    req.body = "{\"filter\": {}}";
    auto response = handler.get_complaints_statistics(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_complaints_statistics_over_time --------
TEST(AnalyticsApiHandlerTest, GetComplaintsStatisticsOverTime) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_stats_over_time";
    cleanup_collection(*db_ptr, collection);

    // Insert a dummy document with a date field.
    // Here, we insert a document with a "date" field. In a real test,
    // use your actual date format and conversion logic.
    std::time_t raw_time = 1672531200;  // e.g. 01-01-2023 00:00:00 (epoch time)
    db_ptr->insert_one(
        collection,
        make_document(
            kvp("category", "Housing"), kvp("sentiment", -0.5),
            kvp("date", bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(raw_time)}))
            .view());

    // Build a request that includes _from_date and _to_date.
    crow::request req;
    req.body =
        "{\"filter\": {\"_from_date\": \"01-01-2023 00:00:00\", \"_to_date\": \"02-01-2023 "
        "00:00:00\"}}";
    auto response = handler.get_complaints_statistics_over_time(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_complaints_statistics_grouped --------
TEST(AnalyticsApiHandlerTest, GetComplaintsStatisticsGrouped) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_stats_grouped";
    cleanup_collection(*db_ptr, collection);

    // Insert dummy documents with different group values.
    db_ptr->insert_one(collection,
                       make_document(kvp("category", "Housing"), kvp("sentiment", -0.5)).view());
    db_ptr->insert_one(collection,
                       make_document(kvp("category", "Housing"), kvp("sentiment", -0.3)).view());
    db_ptr->insert_one(
        collection, make_document(kvp("category", "Transportation"), kvp("sentiment", 0.7)).view());

    // Build a request with a group_by_field.
    crow::request req;
    req.body = "{\"group_by_field\": \"category\", \"filter\": {}}";
    auto response = handler.get_complaints_statistics_grouped(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_complaints_statistics_grouped_over_time --------
TEST(AnalyticsApiHandlerTest, GetComplaintsStatisticsGroupedOverTime) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_stats_grouped_over_time";
    cleanup_collection(*db_ptr, collection);

    // Insert a dummy document with a date field and a grouping field.
    std::time_t raw_time = 1672531200;  // 01-01-2023 00:00:00
    db_ptr->insert_one(
        collection,
        make_document(
            kvp("category", "Housing"), kvp("sentiment", -0.5),
            kvp("date", bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(raw_time)}))
            .view());

    // Build a request with group_by_field and required date fields.
    crow::request req;
    req.body =
        "{\"group_by_field\": \"category\", \"filter\": {\"_from_date\": \"01-01-2023 00:00:00\", "
        "\"_to_date\": \"02-01-2023 00:00:00\"}}";
    auto response = handler.get_complaints_statistics_grouped_over_time(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_complaints_statistics_grouped_by_sentiment_value --------
TEST(AnalyticsApiHandlerTest, GetComplaintsStatisticsGroupedBySentimentValue) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    AnalyticsApiHandler handler;
    std::string collection = "test_analytics_stats_grouped_by_sentiment";
    cleanup_collection(*db_ptr, collection);

    // Insert dummy complaint documents.
    db_ptr->insert_one(collection, make_document(kvp("sentiment", -0.7)).view());
    db_ptr->insert_one(collection, make_document(kvp("sentiment", -0.65)).view());
    db_ptr->insert_one(collection, make_document(kvp("sentiment", 0.3)).view());
    db_ptr->insert_one(collection, make_document(kvp("sentiment", 0.6)).view());

    // Build a request with a bucket_size for sentiment grouping and required date fields.
    crow::request req;
    req.body =
        "{\"bucket_size\": 0.5, \"filter\": {\"_from_date\": \"01-01-2010 00:00:00\", "
        "\"_to_date\": \"31-12-2010 23:59:59\"}}";
    auto response =
        handler.get_complaints_statistics_grouped_by_sentiment_value(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}
