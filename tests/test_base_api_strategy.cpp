// test_base_api_strategy.cpp

#include "base_api_strategy.hpp"
#include "base_api_strategy_utils.hpp"

#include <gtest/gtest.h>
#include <bsoncxx/builder/basic/document.hpp>
// #include <bsoncxx/types/value.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
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
// Tests for Response Processing functions that do not require complex result types.
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

//----------------------------------------------------------------------------
// Stub classes to simulate mongocxx result objects for testing response functions.
// These stubs now construct valid BSON documents with the expected keys.
//----------------------------------------------------------------------------

// namespace {
//     // Stub for mongocxx::result::insert_one.
//     // Use a valid ObjectID string (24 hex digits).
//     struct StubInsertOneResult {
//         bsoncxx::types::bson_value::value stub_oid;
    
//         explicit StubInsertOneResult(const std::string& id_str)
//             : stub_oid{bsoncxx::oid{id_str}} { }
    
//         // Mimic the interface expected by process_response_func_insert_one
//         const bsoncxx::types::bson_value::value& inserted_id() const noexcept {
//             return stub_oid;
//         }
//     };
    

//     // Stub for mongocxx::result::delete_result.
//     struct StubDeleteResult {
//         std::uint64_t count;
//         bsoncxx::document::value result_doc;
//         // Construct a valid BSON document with key "nRemoved".
//         StubDeleteResult(std::uint64_t c)
//             : count(c),
//               result_doc(make_document(kvp("nRemoved", static_cast<int32_t>(c)))) { }
//         std::uint64_t deleted_count() const {
//             return count;
//         }
//         // Provide a view() method to allow the process function to read the document.
//         bsoncxx::document::view view() const {
//             return result_doc.view();
//         }
//     };

//     // Stub for mongocxx::result::update.
//     struct StubUpdateResult {
//         std::uint64_t matched;
//         std::uint64_t modified;
//         std::uint64_t upserted;
//         bsoncxx::document::value result_doc;
//         // Construct a valid BSON document with keys "nMatched", "nModified", and "nUpserted".
//         StubUpdateResult(std::uint64_t m, std::uint64_t mod, std::uint64_t up)
//             : matched(m), modified(mod), upserted(up),
//               result_doc(make_document(
//                   kvp("nMatched", static_cast<int32_t>(m)),
//                   kvp("nModified", static_cast<int32_t>(mod)),
//                   kvp("nUpserted", static_cast<int32_t>(up))
//               )) { }
//         std::uint64_t matched_count() const { return matched; }
//         std::uint64_t modified_count() const { return modified; }
//         std::uint64_t upserted_count() const { return upserted; }
//         bsoncxx::document::view view() const {
//             return result_doc.view();
//         }
//     };
// }

//----------------------------------------------------------------------------
// Tests for Response Processing functions that use mongocxx::result types.
//----------------------------------------------------------------------------

// TEST(BaseApiStrategyTest, ProcessResponseInsertOne) {
//     // Create the stub with a valid ObjectID string.
//     StubInsertOneResult stub_result("5f1d7f8603a1b70569a29fbc");

//     // Modify your BaseApiStrategy or introduce an overload to accept your stub type.
//     // For example, if you add an overload:
//     // crow::json::wvalue process_response_func_insert_one(const StubInsertOneResult& result);
//     crow::json::wvalue w_response = BaseApiStrategy::process_response_func_insert_one(stub_result);

//     crow::json::rvalue response = crow::json::load(w_response.dump());
//     EXPECT_TRUE(response.has("oid"));
//     EXPECT_EQ(response["oid"].s(), std::string("5f1d7f8603a1b70569a29fbc"));
// }
// TEST(BaseApiStrategyTest, ProcessResponseDeleteOne) {
//     // Create a stub delete result returning 5 deleted documents.
//     StubDeleteResult stub_del(5);
//     const mongocxx::result::delete_result& stub_delete =
//         *reinterpret_cast<const mongocxx::result::delete_result*>(&stub_del);
    
//     crow::json::wvalue w_response = BaseApiStrategy::process_response_func_delete_one(stub_delete);
//     crow::json::rvalue response = crow::json::load(w_response.dump());
//     EXPECT_TRUE(response.has("deleted_count"));
//     EXPECT_EQ(response["deleted_count"].i(), 5);
// }

// TEST(BaseApiStrategyTest, ProcessResponseDeleteMany) {
//     // Process delete_many uses the same response function.
//     StubDeleteResult stub_del(7);
//     const mongocxx::result::delete_result& stub_delete =
//         *reinterpret_cast<const mongocxx::result::delete_result*>(&stub_del);
    
//     crow::json::wvalue w_response = BaseApiStrategy::process_response_func_delete_many(stub_delete);
//     crow::json::rvalue response = crow::json::load(w_response.dump());
//     EXPECT_TRUE(response.has("deleted_count"));
//     EXPECT_EQ(response["deleted_count"].i(), 7);
// }

// TEST(BaseApiStrategyTest, ProcessResponseUpdateOne) {
//     // Create a stub update result.
//     StubUpdateResult stub_update(5, 3, 1);
//     const mongocxx::result::update& stub_upd =
//         *reinterpret_cast<const mongocxx::result::update*>(&stub_update);
    
//     crow::json::wvalue w_response = BaseApiStrategy::process_response_func_update_one(stub_upd);
//     crow::json::rvalue response = crow::json::load(w_response.dump());
//     EXPECT_TRUE(response.has("matched_count"));
//     EXPECT_TRUE(response.has("modified_count"));
//     EXPECT_TRUE(response.has("upserted_count"));
//     EXPECT_EQ(response["matched_count"].i(), 5);
//     EXPECT_EQ(response["modified_count"].i(), 3);
//     EXPECT_EQ(response["upserted_count"].i(), 1);
// }