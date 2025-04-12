#include "base_api_strategy.hpp"
#include "base_api_strategy_utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <gtest/gtest.h>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/options/delete.hpp>

#include <tuple>
#include <vector>
#include <string>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

//----------------------------------------------------------------------------
// Tests for Request Processing functions
//----------------------------------------------------------------------------

TEST(BaseApiStrategyTest, ProcessRequestGetOne) {
    // Create a request with a "filter" field.
    crow::request req;
    req.body = R"({"filter": {"key": "value"}})";
    
    // This should pass validation and produce a BSON document from the given filter.
    auto result = BaseApiStrategy::process_request_func_get_one(req);
    auto filter_bson = std::get<0>(result);
    std::string filter_json = bsoncxx::to_json(filter_bson.view());
    // Check that the JSON contains the "key": "value" pair.
    EXPECT_NE(filter_json.find("\"key\""), std::string::npos);
    EXPECT_NE(filter_json.find("\"value\""), std::string::npos);
}

TEST(BaseApiStrategyTest, ProcessRequestInsertOne) {
    // Create a request with a "document" field.
    crow::request req;
    req.body = R"({"document": {"key": "value"}})";
    
    auto result = BaseApiStrategy::process_request_func_insert_one(req);
    auto document_bson = std::get<0>(result);
    std::string doc_json = bsoncxx::to_json(document_bson.view());
    // Check that the resulting JSON contains the expected field.
    EXPECT_NE(doc_json.find("\"key\""), std::string::npos);
    EXPECT_NE(doc_json.find("\"value\""), std::string::npos);
}

TEST(BaseApiStrategyTest, ProcessRequestCountDocuments) {
    // Create a request with a "filter" field.
    crow::request req;
    req.body = R"({"filter": {"key": "value"}})";
    
    auto result = BaseApiStrategy::process_request_func_count_documents(req);
    auto filter_bson = std::get<0>(result);
    std::string filter_json = bsoncxx::to_json(filter_bson.view());
    EXPECT_NE(filter_json.find("\"key\""), std::string::npos);
    EXPECT_NE(filter_json.find("\"value\""), std::string::npos);
}

//----------------------------------------------------------------------------
// Tests for Response Processing functions 
//----------------------------------------------------------------------------

TEST(BaseApiStrategyTest, ProcessResponseGetOne) {
    // Create a sample BSON document.
    auto doc = make_document(kvp("dummy", 1));
    // Process the document.
    crow::json::wvalue w_response = BaseApiStrategy::process_response_func_get_one(doc);
    // Convert wvalue to rvalue to perform checks.
    crow::json::rvalue response = crow::json::load(w_response.dump());
    // We expect the returned JSON to have a "document" field.
    EXPECT_TRUE(response.has("document"));
    // Depending on the implementation, we might expect the "document" to contain the field "dummy".
    std::string subdoc_str = w_response.dump();
    EXPECT_NE(subdoc_str.find("dummy"), std::string::npos);
}

TEST(BaseApiStrategyTest, ProcessResponseCountDocuments) {
    // Test that when count is given, the returned JSON contains the correct count.
    long long count = 42;
    crow::json::wvalue w_response = BaseApiStrategy::process_response_func_count_documents(count);
    crow::json::rvalue response = crow::json::load(w_response.dump());
    EXPECT_TRUE(response.has("count"));
    // The count field should equal 42.
    EXPECT_EQ(response["count"].i(), 42);
}