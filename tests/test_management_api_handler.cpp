#include "management_api_handler.hpp"
#include "database_manager.hpp"
#include "crow.h"
#include <gtest/gtest.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <chrono>
#include <thread>
#include <string>
#include <tuple>
#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Helper to clear all documents in a collection.
static inline void cleanup_collection(DatabaseManager& dbManager, const std::string& collection_name) {
    dbManager.delete_many(collection_name, make_document().view());
}

// -------- Test for get_one_by_oid --------
TEST(ManagementApiHandlerTest, GetOneByOidFound) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_get_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document and retrieve its oid.
    auto doc = make_document(kvp("get_one_test", true), kvp("value", 123));
    auto insert_result = db_ptr->insert_one(collection, doc.view());
    ASSERT_TRUE(insert_result.has_value());
    std::string oid_str = insert_result->inserted_id().get_oid().value.to_string();

    // Build request with the oid.
    crow::request req;
    req.body = "{\"oid\": \"" + oid_str + "\"}";

    auto response = handler.get_one_by_oid(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    // Check that the response body indicates a successful get request.
    EXPECT_NE(response.body.find("Server processed get request successfully"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_all --------
TEST(ManagementApiHandlerTest, GetAll) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_get_all";
    cleanup_collection(*db_ptr, collection);

    // Insert multiple documents.
    db_ptr->insert_one(collection, make_document(kvp("get_all_test", true), kvp("value", 1)).view());
    db_ptr->insert_one(collection, make_document(kvp("get_all_test", true), kvp("value", 2)).view());

    // Empty request body works for get_all.
    crow::request req;
    req.body = "{}";
    auto response = handler.get_all(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("\"documents\""), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_by_daterange --------
TEST(ManagementApiHandlerTest, GetByDaterange) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_get_by_daterange";
    cleanup_collection(*db_ptr, collection);

    // Insert a document with a date field.
    // Create a test date: February 1, 2020.
    std::time_t time_raw = 1580515200; 
    std::chrono::milliseconds time_ms{time_raw * 1000};
    db_ptr->insert_one(collection, 
        make_document(kvp("some_key", "some_value"), 
                      kvp("date", bsoncxx::types::b_date{time_ms})).view());

    // Build request with start_date and end_date.
    crow::request req;
    req.body = "{\"start_date\": \"01-01-2020 00:00:00\", \"end_date\": \"31-12-2020 23:59:59\"}";
    auto response = handler.get_by_daterange(req, db_ptr, collection);
    std::cout << response.body << std::endl;
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("\"documents\""), std::string::npos);
    EXPECT_NE(response.body.find("\"some_key\""), std::string::npos);
    EXPECT_NE(response.body.find("\"some_value\""), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_many --------
TEST(ManagementApiHandlerTest, GetMany) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_get_many";
    cleanup_collection(*db_ptr, collection);

    // Insert multiple documents that match our filter.
    db_ptr->insert_one(collection, make_document(kvp("get_many_test", true), kvp("value", 1)).view());
    db_ptr->insert_one(collection, make_document(kvp("get_many_test", true), kvp("value", 2)).view());
    db_ptr->insert_one(collection, make_document(kvp("get_many_test", true), kvp("value", 3)).view());

    // Build request: filter, page_size, page_number and optional sort.
    crow::request req;
    req.body = "{\"filter\": {\"get_many_test\": true}, \"page_size\": 2, \"page_number\": 1, \"sort\": {}}";
    auto response = handler.get_many(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("\"documents\""), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for get_statistics_poll_responses --------
TEST(ManagementApiHandlerTest, GetStatisticsPollResponses) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_get_statistics";
    cleanup_collection(*db_ptr, collection);

    // Insert documents for poll responses.
    db_ptr->insert_one(collection, make_document(kvp("poll_test", true), kvp("response", "yes")).view());
    db_ptr->insert_one(collection, make_document(kvp("poll_test", true), kvp("response", "no")).view());
    db_ptr->insert_one(collection, make_document(kvp("poll_test", true), kvp("response", "yes")).view());

    // Build request with filter.
    crow::request req;
    req.body = "{\"filter\": {\"poll_test\": true}}";
    auto response = handler.get_statistics_poll_responses(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("\"statistics\""), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for delete_one_by_oid --------
TEST(ManagementApiHandlerTest, DeleteOneByOid) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_delete_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document to be deleted.
    auto insert_result = db_ptr->insert_one(collection, make_document(kvp("delete_one_test", true)).view());
    ASSERT_TRUE(insert_result.has_value());
    std::string oid_str = insert_result->inserted_id().get_oid().value.to_string();

    // Build request with the oid.
    crow::request req;
    req.body = "{\"oid\": \"" + oid_str + "\"}";
    auto response = handler.delete_one_by_oid(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed delete request successfully"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for delete_many_by_oids --------
TEST(ManagementApiHandlerTest, DeleteManyByOids) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_delete_many";
    cleanup_collection(*db_ptr, collection);

    // Insert multiple documents.
    auto insert_result1 = db_ptr->insert_one(collection, make_document(kvp("delete_many_test", true)).view());
    auto insert_result2 = db_ptr->insert_one(collection, make_document(kvp("delete_many_test", true)).view());
    ASSERT_TRUE(insert_result1.has_value());
    ASSERT_TRUE(insert_result2.has_value());
    std::string oid_str1 = insert_result1->inserted_id().get_oid().value.to_string();
    std::string oid_str2 = insert_result2->inserted_id().get_oid().value.to_string();

    // Build request with an array of oids.
    crow::request req;
    req.body = "{\"oids\": [\"" + oid_str1 + "\", \"" + oid_str2 + "\"]}";
    auto response = handler.delete_many_by_oids(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed delete request successfully"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for update_one_by_oid --------
TEST(ManagementApiHandlerTest, UpdateOneByOid) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    ManagementApiHandler handler;
    std::string collection = "test_management_update_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document to update.
    auto insert_result = db_ptr->insert_one(collection, make_document(kvp("update_test", true), kvp("status", "old")).view());
    ASSERT_TRUE(insert_result.has_value());
    std::string oid_str = insert_result->inserted_id().get_oid().value.to_string();

    // Build request with the oid and the update document.
    crow::request req;
    req.body = "{\"oid\": \"" + oid_str + "\", \"update_document\": {\"$set\": {\"status\": \"updated\"}}}";
    auto response = handler.update_one_by_oid(req, db_ptr, collection);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed update request successfully"), std::string::npos);

    // Optionally, verify the update by querying the document from the database.

    cleanup_collection(*db_ptr, collection);
}
