#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/update.hpp>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "crow.h"
#include "gtest/gtest.h"
#include "management_api_strategy.hpp"

using bsoncxx::to_json;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// =====================
// Tests for processing request functions
// =====================

// -------- Test for process_request_func_get_one_by_oid --------
TEST(ManagementApiStrategyTest, ProcessRequestGetOneByOid) {
    crow::request req;
    std::string oid = "507f1f77bcf86cd799439011";
    req.body = "{\"oid\": \"" + oid + "\"}";

    // Call the function and unpack the tuple.
    auto result = ManagementApiStrategy::process_request_func_get_one_by_oid(req);
    auto filter = std::get<0>(result);
    // Convert filter to JSON string for verification.
    std::string filter_json = to_json(filter.view());

    // Check that the filter contains the provided oid value.
    EXPECT_NE(filter_json.find(oid), std::string::npos);
}

// -------- Test for process_request_func_get_many --------
TEST(ManagementApiStrategyTest, ProcessRequestGetMany) {
    crow::request req;
    req.body =
        "{\"filter\": {\"test\": true}, \"page_size\": 5, \"page_number\": 1, \"sort\": "
        "{\"value\": 1}}";

    auto result = ManagementApiStrategy::process_request_func_get_many(req);
    auto filter = std::get<0>(result);
    auto options = std::get<1>(result);
    auto sort = std::get<2>(result);

    std::string filter_json = to_json(filter.view());
    std::string sort_json = to_json(sort.view());

    // Verify that the filter has the key "test" set to true.
    EXPECT_NE(filter_json.find("test"), std::string::npos);
    EXPECT_NE(filter_json.find("true"), std::string::npos);
    // Verify that the sort document contains the expected sort field.
    EXPECT_NE(sort_json.find("value"), std::string::npos);
    EXPECT_NE(sort_json.find("1"), std::string::npos);
}

// -------- Test for process_request_func_get_all --------
TEST(ManagementApiStrategyTest, ProcessRequestGetAll) {
    crow::request req;
    req.body = "{}";  // No fields required.
    auto result = ManagementApiStrategy::process_request_func_get_all(req);
    auto filter = std::get<0>(result);
    auto sort = std::get<2>(result);

    std::string filter_json = to_json(filter.view());
    std::string sort_json = to_json(sort.view());

    // Expect empty filter and sort documents.
    EXPECT_EQ(filter_json, "{ }");
    EXPECT_EQ(sort_json, "{ }");
}

// -------- Test for process_request_func_get_by_daterange --------
TEST(ManagementApiStrategyTest, ProcessRequestGetByDaterange) {
    crow::request req;
    req.body = "{\"start_date\": \"01-01-2022 00:00:00\", \"end_date\": \"01-01-2022 00:00:01\"}";

    auto result = ManagementApiStrategy::process_request_func_get_by_daterange(req);
    auto filter = std::get<0>(result);
    std::string filter_json = to_json(filter.view());

    // Expect the filter to include "date" with both $gte and $lte.
    EXPECT_NE(filter_json.find("\"$gte\""), std::string::npos);
    EXPECT_NE(filter_json.find("1640995200000"),
              std::string::npos);  // unix timestamp for 01-01-2022 00:00:00
    EXPECT_NE(filter_json.find("\"$lte\""), std::string::npos);
    EXPECT_NE(filter_json.find("1640995201000"),
              std::string::npos);  // unix timestamp for 01-01-2022 00:00:01
}

// -------- Test for process_request_func_delete_one_by_oid --------
TEST(ManagementApiStrategyTest, ProcessRequestDeleteOneByOid) {
    crow::request req;
    std::string oid = "507f1f77bcf86cd799439012";
    req.body = "{\"oid\": \"" + oid + "\"}";

    auto result = ManagementApiStrategy::process_request_func_delete_one_by_oid(req);
    auto filter = std::get<0>(result);
    std::string filter_json = to_json(filter.view());

    EXPECT_NE(filter_json.find(oid), std::string::npos);
}

// -------- Test for process_request_func_delete_many_by_oids --------
TEST(ManagementApiStrategyTest, ProcessRequestDeleteManyByOids) {
    crow::request req;
    req.body = "{\"oids\": [\"507f1f77bcf86cd799439013\", \"507f1f77bcf86cd799439014\"]}";

    auto result = ManagementApiStrategy::process_request_func_delete_many_by_oids(req);
    auto filter = std::get<0>(result);
    std::string filter_json = to_json(filter.view());

    EXPECT_NE(filter_json.find("507f1f77bcf86cd799439013"), std::string::npos);
    EXPECT_NE(filter_json.find("507f1f77bcf86cd799439014"), std::string::npos);
}

// -------- Test for process_request_func_update_one_by_oid --------
TEST(ManagementApiStrategyTest, ProcessRequestUpdateOneByOid) {
    crow::request req;
    std::string oid = "507f1f77bcf86cd799439015";
    req.body =
        "{\"oid\": \"" + oid + "\", \"update_document\": {\"$set\": {\"field\": \"new_value\"}}}";

    auto result = ManagementApiStrategy::process_request_func_update_one_by_oid(req);
    auto filter = std::get<0>(result);
    auto update_doc = std::get<1>(result);
    std::string filter_json = to_json(filter.view());
    std::string update_doc_json = to_json(update_doc.view());

    EXPECT_NE(filter_json.find(oid), std::string::npos);
    EXPECT_NE(update_doc_json.find("field"), std::string::npos);
    EXPECT_NE(update_doc_json.find("new_value"), std::string::npos);
}

// -------- Test for process_request_func_get_statistics_poll_responses --------
TEST(ManagementApiStrategyTest, ProcessRequestGetStatisticsPollResponses) {
    crow::request req;
    req.body = "{\"filter\": {\"poll\": true}}";

    auto result = ManagementApiStrategy::process_request_func_get_statistics_poll_responses(req);
    auto filter = std::get<0>(result);
    std::string filter_json = to_json(filter.view());

    EXPECT_NE(filter_json.find("poll"), std::string::npos);
    EXPECT_NE(filter_json.find("true"), std::string::npos);
}
