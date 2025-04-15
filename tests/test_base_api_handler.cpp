#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <cstdlib>
#include <functional>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <tuple>
#include <vector>

#include "base_api_handler.hpp"
#include "constants.hpp"
#include "crow.h"
#include "database_manager.hpp"
#include "env_manager.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Helper to clear all documents in a collection.
static inline void cleanup_collection(DatabaseManager& dbManager,
                                      const std::string& collection_name) {
    dbManager.delete_many(collection_name, make_document().view());
}

// -------- Test for find_one --------
TEST(BaseApiHandlerTest, FindOneFound) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_find_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document that should be matched.
    auto doc = make_document(kvp("find_one_test", true), kvp("value", 123));
    auto insert_result = db_ptr->insert_one(collection, doc.view());
    ASSERT_TRUE(insert_result.has_value());

    // Dummy lambda: process_request_func for find_one returns a filter matching our document.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
        mongocxx::options::find options{};
        return {make_document(kvp("find_one_test", true)), options};
    };

    // Dummy lambda: process_response_func converts the found document into a simple JSON.
    auto process_response_func = [](const bsoncxx::document::value& doc) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["found"] = "yes";
        return json;
    };

    crow::request req;
    auto response =
        handler.find_one(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed get request successfully"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for find (multiple documents) --------
TEST(BaseApiHandlerTest, FindMultiple) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_find";
    cleanup_collection(*db_ptr, collection);

    // Insert several documents.
    db_ptr->insert_one(collection, make_document(kvp("find_test", true), kvp("value", 1)).view());
    db_ptr->insert_one(collection, make_document(kvp("find_test", true), kvp("value", 2)).view());
    db_ptr->insert_one(collection, make_document(kvp("find_test", false), kvp("value", 3)).view());

    // Dummy lambda: process_request_func returns filter, options, and sort (empty here).
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::find, bsoncxx::document::value> {
        mongocxx::options::find options{};
        return {make_document(kvp("find_test", true)), options, make_document()};
    };

    // Dummy lambda: process_response_func counts the number of documents in the cursor.
    auto process_response_func = [](mongocxx::cursor& cursor) -> crow::json::wvalue {
        int count = 0;
        for (auto&& doc : cursor) {
            ++count;
        }
        crow::json::wvalue json;
        json["found_count"] = count;
        return json;
    };

    crow::request req;
    auto response =
        handler.find(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("\"found_count\":"), std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for insert_one --------
TEST(BaseApiHandlerTest, InsertOne) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_insert_one";
    cleanup_collection(*db_ptr, collection);

    // Dummy lambda: process_request_func for insert_one returns the document to insert.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
        mongocxx::options::insert options{};
        return {make_document(kvp("insert_test", true), kvp("data", "dummy")), options};
    };

    // Dummy lambda: process_response_func converts the insert_one result into a JSON response.
    auto process_response_func =
        [](const mongocxx::result::insert_one& result) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["inserted"] = "yes";
        return json;
    };

    crow::request req;
    auto response =
        handler.insert_one(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed insert request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for delete_one --------
TEST(BaseApiHandlerTest, DeleteOne) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_delete_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document to be deleted.
    db_ptr->insert_one(collection, make_document(kvp("delete_test", true)).view());

    // Dummy lambda: process_request_func for delete_one returns the filter.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options> {
        mongocxx::options::delete_options options{};
        return {make_document(kvp("delete_test", true)), options};
    };

    // Dummy lambda: process_response_func returns JSON with the number of deleted documents.
    auto process_response_func =
        [](const mongocxx::result::delete_result& result) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["deleted"] = result.deleted_count();
        return json;
    };

    crow::request req;
    auto response =
        handler.delete_one(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed delete request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for delete_many --------
TEST(BaseApiHandlerTest, DeleteMany) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_delete_many";
    cleanup_collection(*db_ptr, collection);

    // Insert multiple documents.
    db_ptr->insert_one(collection, make_document(kvp("delete_many_test", true)).view());
    db_ptr->insert_one(collection, make_document(kvp("delete_many_test", true)).view());

    // Dummy lambda: process_request_func for delete_many.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::delete_options> {
        mongocxx::options::delete_options options{};
        return {make_document(kvp("delete_many_test", true)), options};
    };

    // Dummy lambda: process_response_func returns number of deleted documents.
    auto process_response_func =
        [](const mongocxx::result::delete_result& result) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["deleted"] = result.deleted_count();
        return json;
    };

    crow::request req;
    auto response =
        handler.delete_many(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed delete request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for update_one --------
TEST(BaseApiHandlerTest, UpdateOne) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_update_one";
    cleanup_collection(*db_ptr, collection);

    // Insert a document to update.
    db_ptr->insert_one(collection,
                       make_document(kvp("update_test", true), kvp("status", "old")).view());

    // Dummy lambda: process_request_func for update_one returns filter, update document, and update
    // options.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, bsoncxx::document::value,
                      mongocxx::options::update> {
        mongocxx::options::update options{};
        auto filter = make_document(kvp("update_test", true));
        auto update_doc = make_document(kvp("$set", make_document(kvp("status", "updated"))));
        return {filter, update_doc, options};
    };

    // Dummy lambda: process_response_func returns counts from the update result.
    auto process_response_func = [](const mongocxx::result::update& result) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["matched"] = result.matched_count();
        json["modified"] = result.modified_count();
        return json;
    };

    crow::request req;
    auto response =
        handler.update_one(req, db_ptr, collection, process_request_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed update request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for count_documents --------
TEST(BaseApiHandlerTest, CountDocuments) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_count_documents";
    cleanup_collection(*db_ptr, collection);

    // Insert two documents.
    db_ptr->insert_one(collection, make_document(kvp("count_test", true)).view());
    db_ptr->insert_one(collection, make_document(kvp("count_test", true)).view());

    // Dummy lambda: process_request_func for count_documents returns filter and options.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<bsoncxx::document::value, mongocxx::options::count> {
        mongocxx::options::count options{};
        return {make_document(kvp("count_test", true)), options};
    };

    // Dummy lambda: process_response_func returns JSON with the count.
    auto process_response_func = [](const long long int& count) -> crow::json::wvalue {
        crow::json::wvalue json;
        json["count"] = count;
        return json;
    };

    crow::request req;
    auto response = handler.count_documents(req, db_ptr, collection, process_request_func,
                                            process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed count_documents request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}

// -------- Test for aggregate --------
TEST(BaseApiHandlerTest, Aggregate) {
    auto db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");
    BaseApiHandler handler;
    std::string collection = "test_handler_aggregate";
    cleanup_collection(*db_ptr, collection);

    // Insert documents for aggregation.
    db_ptr->insert_one(
        collection,
        make_document(kvp("agg_test", true), kvp("group", "A"), kvp("score", 10)).view());
    db_ptr->insert_one(
        collection,
        make_document(kvp("agg_test", true), kvp("group", "A"), kvp("score", 20)).view());
    db_ptr->insert_one(
        collection,
        make_document(kvp("agg_test", true), kvp("group", "B"), kvp("score", 30)).view());

    // Dummy lambda: process_request_func for aggregate returns a vector of documents and aggregate
    // options.
    auto process_request_func = [](const crow::request& req)
        -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
        mongocxx::options::aggregate options{};
        std::vector<bsoncxx::document::value> docs;
        docs.push_back(make_document(kvp("agg_test", true)));
        return {docs, options};
    };

    // Dummy lambda: create_pipeline_func builds an aggregation pipeline.
    auto create_pipeline_func =
        [](const std::vector<bsoncxx::document::value>& docs) -> mongocxx::pipeline {
        mongocxx::pipeline pipeline{};
        pipeline.match(make_document(kvp("agg_test", true)).view());
        pipeline.group(
            make_document(kvp("_id", "$group"), kvp("total", make_document(kvp("$sum", "$score"))))
                .view());
        return pipeline;
    };

    // Dummy lambda: process_response_func counts the number of groups returned.
    auto process_response_func = [](const crow::request& req,
                                    mongocxx::cursor& cursor) -> crow::json::wvalue {
        int groupCount = 0;
        for (auto&& doc : cursor) {
            ++groupCount;
        }
        crow::json::wvalue json;
        json["groups"] = groupCount;
        return json;
    };

    crow::request req;
    auto response = handler.aggregate(req, db_ptr, collection, process_request_func,
                                      create_pipeline_func, process_response_func);
    EXPECT_EQ(response.code, 200);
    EXPECT_NE(response.body.find("Server processed aggregate request successfully"),
              std::string::npos);

    cleanup_collection(*db_ptr, collection);
}