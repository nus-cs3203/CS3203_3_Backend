#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <cstdlib>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <vector>

#include "constants.hpp"
#include "crow.h"
#include "database_manager.hpp"
#include "env_manager.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Helper function to clean up a collection by deleting all documents.
void cleanup_collection(DatabaseManager& dbManager, const std::string& collection_name) {
    // Delete all documents by passing an empty document as filter.
    dbManager.delete_many(collection_name, make_document().view());
}

// ----- Test for insert_one and find_one -----
TEST(DatabaseManagerTest, InsertOneAndFindOne) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_insert_one";
    cleanup_collection(dbManager, collection_name);

    // Build a document.
    auto doc = make_document(kvp("name", "Alice"));
    auto insert_result = dbManager.insert_one(collection_name, doc.view());
    ASSERT_TRUE(insert_result.has_value()) << "insert_one should return a valid result.";

    // Build filter document.
    auto filter = make_document(kvp("name", "Alice"));
    auto found = dbManager.find_one(collection_name, filter.view());
    ASSERT_TRUE(found.has_value()) << "find_one should locate the inserted document.";

    std::string found_json = bsoncxx::to_json(found->view());
    EXPECT_NE(found_json.find("Alice"), std::string::npos)
        << "Inserted document should contain 'Alice'.";

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for insert_many and count_documents -----
TEST(DatabaseManagerTest, InsertManyAndCount) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_insert_many";
    cleanup_collection(dbManager, collection_name);

    // Create multiple documents.
    std::vector<bsoncxx::document::value> docs;
    docs.push_back(make_document(kvp("item", 1)));
    docs.push_back(make_document(kvp("item", 2)));

    auto insert_many_result = dbManager.insert_many(collection_name, docs);
    ASSERT_TRUE(insert_many_result.has_value()) << "insert_many should return a valid result.";

    long long count = dbManager.count_documents(collection_name, make_document().view());
    EXPECT_EQ(count, 2) << "There should be 2 documents in the collection.";

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for update_one -----
TEST(DatabaseManagerTest, UpdateOne) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_update_one";
    cleanup_collection(dbManager, collection_name);

    // Insert a document with status "pending".
    auto doc = make_document(kvp("status", "pending"));
    auto insert_result = dbManager.insert_one(collection_name, doc.view());
    ASSERT_TRUE(insert_result.has_value());

    // Update the document to set status to "complete".
    auto filter = make_document(kvp("status", "pending"));
    auto update_doc = make_document(kvp("$set", make_document(kvp("status", "complete"))));
    auto update_result = dbManager.update_one(collection_name, filter.view(), update_doc.view());
    ASSERT_TRUE(update_result.has_value()) << "update_one should return a valid result.";

    // Verify that a document with status "complete" exists.
    auto found =
        dbManager.find_one(collection_name, make_document(kvp("status", "complete")).view());
    ASSERT_TRUE(found.has_value()) << "Updated document should be found.";

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for update_many -----
TEST(DatabaseManagerTest, UpdateMany) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_update_many";
    cleanup_collection(dbManager, collection_name);

    // Insert multiple documents with various categories.
    std::vector<bsoncxx::document::value> docs;
    docs.push_back(make_document(kvp("category", "A"), kvp("value", 10)));
    docs.push_back(make_document(kvp("category", "A"), kvp("value", 20)));
    docs.push_back(make_document(kvp("category", "B"), kvp("value", 30)));
    auto insert_many_result = dbManager.insert_many(collection_name, docs);
    ASSERT_TRUE(insert_many_result.has_value());

    // Update: For documents in category "A", increment "value" by 5.
    auto filter = make_document(kvp("category", "A"));
    auto update_doc = make_document(kvp("$inc", make_document(kvp("value", 5))));
    auto update_result = dbManager.update_many(collection_name, filter.view(), update_doc.view());
    ASSERT_TRUE(update_result.has_value());

    // Verify that two documents remain in category "A".
    long long count = dbManager.count_documents(collection_name, filter.view());
    EXPECT_EQ(count, 2);

    // Optionally, check that one document now has "value" equal to 15.
    auto found = dbManager.find_one(collection_name,
                                    make_document(kvp("category", "A"), kvp("value", 15)).view());
    ASSERT_TRUE(found.has_value());

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for delete_one -----
TEST(DatabaseManagerTest, DeleteOne) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_delete_one";
    cleanup_collection(dbManager, collection_name);

    auto doc = make_document(kvp("temp", "to_delete"));
    auto insert_result = dbManager.insert_one(collection_name, doc.view());
    ASSERT_TRUE(insert_result.has_value());

    auto filter = make_document(kvp("temp", "to_delete"));
    auto delete_result = dbManager.delete_one(collection_name, filter.view());
    ASSERT_TRUE(delete_result.has_value());

    long long count = dbManager.count_documents(collection_name, make_document().view());
    EXPECT_EQ(count, 0);
}

// ----- Test for delete_many -----
TEST(DatabaseManagerTest, DeleteMany) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_delete_many";
    cleanup_collection(dbManager, collection_name);

    // Insert multiple documents.
    std::vector<bsoncxx::document::value> docs;
    docs.push_back(make_document(kvp("delete", 1)));
    docs.push_back(make_document(kvp("delete", 2)));
    auto insert_many_result = dbManager.insert_many(collection_name, docs);
    ASSERT_TRUE(insert_many_result.has_value());

    // Delete all documents.
    auto delete_result = dbManager.delete_many(collection_name, make_document().view());
    ASSERT_TRUE(delete_result.has_value());

    long long count = dbManager.count_documents(collection_name, make_document().view());
    EXPECT_EQ(count, 0);
}

// ----- Test for count_documents -----
TEST(DatabaseManagerTest, CountDocuments) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_count_docs";
    cleanup_collection(dbManager, collection_name);

    // Insert two documents.
    std::vector<bsoncxx::document::value> docs;
    docs.push_back(make_document(kvp("field", "value")));
    docs.push_back(make_document(kvp("field", "value")));
    auto insert_many_result = dbManager.insert_many(collection_name, docs);
    ASSERT_TRUE(insert_many_result.has_value());

    long long count = dbManager.count_documents(collection_name, make_document().view());
    EXPECT_EQ(count, 2);

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for aggregate -----
// Note: This test uses mongocxx::pipeline. If your driver version requires including
// <mongocxx/pipeline.hpp>, ensure that it is available in your project setup.
TEST(DatabaseManagerTest, Aggregate) {
    DatabaseManager dbManager("mongodb://localhost:27017", "test_db");
    std::string collection_name = "test_aggregate";
    cleanup_collection(dbManager, collection_name);

    // Insert multiple documents.
    std::vector<bsoncxx::document::value> docs;
    docs.push_back(make_document(kvp("category", "X"), kvp("score", 10)));
    docs.push_back(make_document(kvp("category", "Y"), kvp("score", 20)));
    docs.push_back(make_document(kvp("category", "X"), kvp("score", 30)));
    auto insert_many_result = dbManager.insert_many(collection_name, docs);
    ASSERT_TRUE(insert_many_result.has_value());

    // Build an aggregation pipeline: match category "X", then group by category and sum scores.
    mongocxx::pipeline pipeline{};
    pipeline.match(make_document(kvp("category", "X")).view());
    pipeline.group(
        make_document(kvp("_id", "$category"), kvp("total", make_document(kvp("$sum", "$score"))))
            .view());

    auto cursor = dbManager.aggregate(collection_name, pipeline);
    int groupCount = 0;
    for (auto&& doc : cursor) {
        ++groupCount;
        std::string json_doc = bsoncxx::to_json(doc);
        EXPECT_NE(json_doc.find("total"), std::string::npos)
            << "Aggregated result should contain the 'total' field.";
    }
    EXPECT_EQ(groupCount, 1);

    cleanup_collection(dbManager, collection_name);
}

// ----- Test for create_from_env -----
TEST(DatabaseManagerTest, CreateFromEnv) {
    // Set environment variables for MongoDB connection.
    setenv("MONGO_URI", "mongodb://localhost:27017", 1);
    setenv("DB_NAME", "test_db", 1);

    auto dbManagerPtr = DatabaseManager::create_from_env();
    ASSERT_NE(dbManagerPtr, nullptr);

    std::string collection_name = "test_create_from_env";
    cleanup_collection(*dbManagerPtr, collection_name);

    auto doc = make_document(kvp("env_test", "value"));
    auto insert_result = dbManagerPtr->insert_one(collection_name, doc.view());
    ASSERT_TRUE(insert_result.has_value());

    auto found =
        dbManagerPtr->find_one(collection_name, make_document(kvp("env_test", "value")).view());
    ASSERT_TRUE(found.has_value());

    cleanup_collection(*dbManagerPtr, collection_name);

    // Clean up environment variables.
    unsetenv("MONGO_URI");
    unsetenv("DB_NAME");
}